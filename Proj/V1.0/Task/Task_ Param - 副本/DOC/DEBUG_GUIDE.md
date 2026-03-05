# Flash 底层驱动调试指南

## 目的
通过逐步的单元测试，验证 `bsp_flash_simple` 的功能，找出参数保存失败的根本原因。

## 测试结构体

```c
typedef struct {
    uint32_t magic;      /* 0xDEADBEEF (4字节) */
    uint32_t seq;        /* 序列号 (4字节) */
    uint16_t value1;     /* 测试值1 (2字节) */
    uint16_t value2;     /* 测试值2 (2字节) */
    uint32_t crc;        /* CRC校验 (4字节) */
    uint8_t  reserved[4];/* 补齐 (4字节) */
} test_record_t;        /* 总计: 20字节 */
```

## 测试步骤

### Step 1: 擦除Flash页
- 调用 `flash_erase_page(NVM_PAGE_A)`
- 预期：返回0表示成功

### Step 2: 验证擦除结果
- 读取Flash，所有字节应为0xFF
- 用途：检查擦除是否完整

### Step 3: 准备数据
- 创建测试记录
- 填充已知的值（magic, seq, value1, value2）
- 计算简单的CRC校验

### Step 4: 写入Flash
- 调用 `flash_write_buffer(addr, data, size)`
- 预期：返回0表示成功
- 关键：验证写入字节数是否等于sizeof()

### Step 5: 立即读回验证
- 读取刚写入的数据
- 逐字段对比，每个字段应完全相同
- 如果失败，说明写入过程有问题

### Step 6: CRC校验
- 重新计算CRC
- 对比存储的CRC与计算值
- 验证数据完整性

### Step 7: 修改并重写
- 修改数据（magic、seq、value1、value2）
- 先擦除后写入
- 验证新值是否覆盖旧值

### Step 8: 模拟掉电重启
- 清空RAM（memset为0）
- 再次读取Flash
- 验证持久化数据是否保留

## 期望的正常输出

```
========== Flash Driver Low-level Test ==========

[Step 1] Erase Flash page A...
  OK: Page erased

[Step 2] Verify erase (all bytes should be 0xFF):
  magic after erase = 0xFFFFFFFF (should be 0xFFFFFFFF)
  seq after erase   = 0xFFFFFFFF (should be 0xFFFFFFFF)

[Step 3] Prepare data to write:
  magic    = 0xDEADBEEF
  seq      = 0x12345678
  value1   = 0xABCD
  value2   = 0xEF01
  crc      = 0x...
  size     = 20 bytes (should be 20)

[Step 4] Write data to Flash...
  OK: Data written (wrote 20 bytes)

[Step 5] Read back immediately:
  magic    = 0xDEADBEEF (expect 0xDEADBEEF) ✓
  seq      = 0x12345678 (expect 0x12345678) ✓
  value1   = 0xABCD (expect 0xABCD) ✓
  value2   = 0xEF01 (expect 0xEF01) ✓
  crc      = 0x... (expect 0x...) ✓

[Step 6] Verify CRC:
  stored CRC   = 0x...
  calculated   = 0x...
  CRC check: ✓ PASS

[Step 7] Modify and write new value:
  new magic = 0xCAFEBABE
  new seq   = 0x87654321
  OK: New data written

[Step 8] Read after 'power cycle' (same Flash data):
  magic    = 0xCAFEBABE (expect 0xCAFEBABE) ✓
  seq      = 0x87654321 (expect 0x87654321) ✓
  value1   = 0x1234 (expect 0x1234) ✓
  value2   = 0x5678 (expect 0x5678) ✓

========== Test Complete ==========
```

## 故障诊断

### 问题1: Step 4 写入失败
**可能原因：**
- 地址未对齐（不是8字节边界）
- Flash被锁定
- Flash控制器故障

**检查方法：**
- 验证 `test_addr` 值（应为 `0x0800F800`）
- 检查 `fmc_unlock()` 是否正确调用

### 问题2: Step 5 读回数据不匹配
**可能原因：**
- 结构体大小不是8字节倍数（写入不完整）
- 字节序问题（大小端）
- 内存对齐问题

**检查方法：**
- 输出 `sizeof(test_record_t)` 应为20
- 逐字节对比Flash内容

### 问题3: Step 6 CRC校验失败
**可能原因：**
- 数据在Flash中被篡改
- CRC计算算法不一致

**检查方法：**
- 转储Flash原始数据
- 手工计算CRC验证

### 问题4: Step 8 数据丢失
**可能原因：**
- Flash页在写入后被意外擦除
- 掉电期间数据未保存
- 多写保护（某些字节无法写入）

**检查方法：**
- 使用JTAG/SWD直接读取Flash
- 检查Flash写保护状态

## 进一步调试步骤

1. **添加详细日志**
   ```c
   printf("Writing %u bytes to addr 0x%08X\n", sizeof(test_record_t), test_addr);
   ```

2. **逐字节验证**
   ```c
   for (int i = 0; i < sizeof(rec_read); i++) {
       if (((uint8_t*)&rec_read)[i] != ((uint8_t*)&rec)[i]) {
           printf("Byte %d mismatch: %02X != %02X\n", i, 
                  ((uint8_t*)&rec_read)[i], ((uint8_t*)&rec)[i]);
       }
   }
   ```

3. **Flash原始转储**
   ```c
   printf("Flash content (hex):\n");
   for (int i = 0; i < sizeof(test_record_t); i++) {
       printf("%02X ", ((uint8_t*)test_addr)[i]);
   }
   printf("\n");
   ```

4. **检查Flash控制器状态**
   - 在写入前检查FMC是否就绪
   - 在写入后检查错误标志

## 完成调试后

当所有步骤都成功（全是✓）后，说明底层Flash驱动正常，可以：
1. 回到 `nvm_param_simple` 层调试
2. 检查结构体大小和CRC计算
3. 验证A/B双页机制



##备份main.c中测试bsp_flash_simple的代码：



/* 测试结构体 */
typedef struct {
    uint32_t magic;      /* 0xDEADBEEF */
    uint32_t seq;        /* 序列号 */
    uint16_t value1;     /* 测试值1 */
    uint16_t value2;     /* 测试值2 */
    uint32_t crc;        /* CRC校验 */
    uint8_t  reserved[4];/* 补齐到24字节: 4+4+2+2+4+4=20 */
} __attribute__((packed)) test_record_t;

/* 简单CRC校验（仅用于演示）*/
static uint32_t simple_crc(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0;
    for (uint32_t i = 0; i < len; i++) {
        crc += data[i];
    }
    return crc;
}

/* CRC覆盖的有效数据大小（不包括crc字段本身和reserved） */
#define CRC_DATA_LEN  (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t))
/* = magic(4) + seq(4) + value1(2) + value2(2) = 12字节 */

int main(void)
{
    test_record_t rec, rec_read;
    uint32_t test_addr = NVM_PAGE_A_ADDR;
    
    /* 系统初始化 */
    systick_config();
    
    printf("\n========== Flash Driver Low-level Test ==========\n\n");
    
    /* ==================== Step 1: 擦除Flash页 ==================== */
    printf("[Step 1] Erase Flash page A...\n");
    if (flash_erase_page(NVM_PAGE_A) == 0) {
        printf("  OK: Page erased\n\n");
    } else {
        printf("  ERROR: Erase failed!\n\n");
        while(1);
    }
    
    /* ==================== Step 2: 验证擦除结果 ==================== */
    printf("[Step 2] Verify erase (all bytes should be 0xFF):\n");
    flash_read_buffer(test_addr, (uint8_t *)&rec_read, sizeof(rec_read));
    printf("  magic after erase = 0x%08X (should be 0xFFFFFFFF)\n", rec_read.magic);
    printf("  seq after erase   = 0x%08X (should be 0xFFFFFFFF)\n\n", rec_read.seq);
    
    /* ==================== Step 3: 准备数据写入 ==================== */
    printf("[Step 3] Prepare data to write:\n");
    /* 先初始化整个结构体为0xFF */
    memset(&rec, 0xFF, sizeof(rec));
    
    /* 然后填充有效数据 */
    rec.magic = 0xDEADBEEF;
    rec.seq = 0x12345678;
    rec.value1 = 0xABCD;
    rec.value2 = 0xEF01;
    
    /* 重要：CRC只覆盖有效数据字段（magic、seq、value1、value2），不包括crc字段本身和reserved */
    rec.crc = simple_crc((uint8_t *)&rec, CRC_DATA_LEN);
    
    printf("  magic    = 0x%08X\n", rec.magic);
    printf("  seq      = 0x%08X\n", rec.seq);
    printf("  value1   = 0x%04X\n", rec.value1);
    printf("  value2   = 0x%04X\n", rec.value2);
    printf("  crc      = 0x%08X (covers %u bytes)\n", rec.crc, CRC_DATA_LEN);
    printf("  size     = %u bytes (should be 20)\n\n", sizeof(rec));
    
    /* ==================== Step 4: 写入Flash ==================== */
    printf("[Step 4] Write data to Flash...\n");
    if (flash_write_buffer(test_addr, (const uint8_t *)&rec, sizeof(rec)) == 0) {
        printf("  OK: Data written (wrote %u bytes)\n\n", sizeof(rec));
    } else {
        printf("  ERROR: Write failed!\n\n");
        while(1);
    }
    
    /* ==================== Step 5: 立即读回验证 ==================== */
    printf("[Step 5] Read back immediately:\n");
    flash_read_buffer(test_addr, (uint8_t *)&rec_read, sizeof(rec_read));
    printf("  magic    = 0x%08X (expect 0x%08X) %s\n", 
           rec_read.magic, rec.magic, 
           rec_read.magic == rec.magic ? "✓" : "✗");
    printf("  seq      = 0x%08X (expect 0x%08X) %s\n", 
           rec_read.seq, rec.seq,
           rec_read.seq == rec.seq ? "✓" : "✗");
    printf("  value1   = 0x%04X (expect 0x%04X) %s\n", 
           rec_read.value1, rec.value1,
           rec_read.value1 == rec.value1 ? "✓" : "✗");
    printf("  value2   = 0x%04X (expect 0x%04X) %s\n", 
           rec_read.value2, rec.value2,
           rec_read.value2 == rec.value2 ? "✓" : "✗");
    printf("  crc      = 0x%08X (expect 0x%08X) %s\n\n", 
           rec_read.crc, rec.crc,
           rec_read.crc == rec.crc ? "✓" : "✗");
    
    /* ==================== Step 6: CRC校验 ==================== */
    printf("[Step 6] Verify CRC:\n");
    /* CRC只覆盖有效数据字段，与写入前计算方式一致 */
    uint32_t calculated_crc = simple_crc((uint8_t *)&rec_read, CRC_DATA_LEN);
    printf("  stored CRC   = 0x%08X\n", rec_read.crc);
    printf("  calculated   = 0x%08X (from %u bytes)\n", calculated_crc, CRC_DATA_LEN);
    printf("  CRC check: %s\n\n", 
           calculated_crc == rec_read.crc ? "✓ PASS" : "✗ FAIL");
    
    /* ==================== Step 7: 修改并重新写入另一个值 ==================== */
    printf("[Step 7] Modify and write new value:\n");
    /* 同样先memset初始化，再填充数据 */
    memset(&rec, 0xFF, sizeof(rec));
    rec.magic = 0xCAFEBABE;
    rec.seq = 0x87654321;
    rec.value1 = 0x1234;
    rec.value2 = 0x5678;
    rec.crc = simple_crc((uint8_t *)&rec, CRC_DATA_LEN);
    
    printf("  new magic = 0x%08X\n", rec.magic);
    printf("  new seq   = 0x%08X\n\n", rec.seq);
    
    /* 先擦除 */
    flash_erase_page(NVM_PAGE_A);
    
    /* 再写入 */
    if (flash_write_buffer(test_addr, (const uint8_t *)&rec, sizeof(rec)) == 0) {
        printf("  OK: New data written\n\n");
    } else {
        printf("  ERROR: Write failed!\n\n");
        while(1);
    }
    
    /* ==================== Step 8: 上电重启模拟 - 读出新值 ==================== */
    printf("[Step 8] Read after 'power cycle' (same Flash data):\n");
    memset(&rec_read, 0, sizeof(rec_read));  /* 清空RAM */
    flash_read_buffer(test_addr, (uint8_t *)&rec_read, sizeof(rec_read));
    printf("  magic    = 0x%08X (expect 0xCAFEBABE) %s\n", 
           rec_read.magic, rec_read.magic == 0xCAFEBABE ? "✓" : "✗");
    printf("  seq      = 0x%08X (expect 0x87654321) %s\n", 
           rec_read.seq, rec_read.seq == 0x87654321 ? "✓" : "✗");
    printf("  value1   = 0x%04X (expect 0x1234) %s\n", 
           rec_read.value1, rec_read.value1 == 0x1234 ? "✓" : "✗");
    printf("  value2   = 0x%04X (expect 0x5678) %s\n\n", 
           rec_read.value2, rec_read.value2 == 0x5678 ? "✓" : "✗");
    
    printf("========== Test Complete ==========\n\n");
    
    /* 主循环 */
    while(1) {
        delay_1ms(1000);
    }
}

/* printf 重定向到 RTT */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}


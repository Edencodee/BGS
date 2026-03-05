# NVM 新增参数操作指南（精简版）

## 适用范围
本指南适用于当前工程的 `nvm_param_simple` 方案（A/B 双页、整结构体存储、CRC32 校验）。

---

## 一、修改结构体

### 1. 在 `sensor_param_t` 中新增字段
- 位置：`BSP/inc/nvm_param_simple.h`
- 示例：新增 `uint8_t mode_ext;`

### 2. 重新计算结构体大小
- 当前 `sensor_param_t` 为 **packed**：
  - 字段总字节数 = 逐项相加
- 记录长度计算公式：
  - `record_size = 4(magic) + 4(seq) + sizeof(sensor_param_t) + 4(crc32) + padding`
- 目标：`record_size` 必须是 **8 字节倍数**。

### 3. 调整 `padding` 大小
- 位置：`BSP/inc/nvm_param_simple.h` 中 `nvm_record_t`
- 计算：
  - `padding = (8 - (record_size % 8)) % 8`
- 更新注释，保持与实际一致。

---

## 二、更新默认值

### 1. 修改出厂默认值
- 位置：`BSP/src/nvm_param_simple.c`
- 对新增字段给出合理默认值

---

## 三、CRC 覆盖范围（关键）

### 规则
- CRC 只覆盖 **magic + seq + param**
- 不包含 `crc32` 字段
- 不包含 `padding`

### 现有实现
- 常量：`NVM_CRC_DATA_LEN`
- 建议保持如下形式：
  - `sizeof(uint32_t) + sizeof(uint32_t) + sizeof(sensor_param_t)`

---

## 四、兼容旧数据（可选但建议）

### 方式一：版本号字段
- 在 `sensor_param_t` 增加 `uint8_t version;`
- 升级后：
  - 读出旧数据时根据 `version` 进行兼容填充

### 方式二：MAGIC 升级
- 变更 `NVM_MAGIC`（例如 `0x5A5AA55B`）
- 触发重新初始化为默认值

---

## 五、测试步骤

1. **编译确认结构体大小**
   - 确认 `sizeof(sensor_param_t)` 变化
   - 确认 `sizeof(nvm_record_t)` 为 8 字节倍数

2. **运行最小用例**
   - 初始化
   - 读默认值
   - 修改+保存
   - 读回验证
   - 恢复默认值

3. **断电重启验证**
   - 断电后再次读取
   - 确认保存值仍然有效

---

## 六、快速检查清单

- [ ] 新字段已加入 `sensor_param_t`
- [ ] `nvm_record_t` `padding` 已重新计算
- [ ] `FACTORY_DEFAULT` 已补齐新字段
- [ ] `NVM_CRC_DATA_LEN` 不包含 padding/crc32
- [ ] 通过最小读写测试

---

## 示例（新增 1 字节字段）

```
// sensor_param_t 增加字段
uint8_t  mode_ext;

// 重新计算 record_size
// 4 + 4 + (原param_size + 1) + 4 + padding

// 调整 padding 为新的值
uint8_t  padding[新的长度];
```

---

如需我根据你当前字段直接算出 `padding` 和默认值修改，请告诉我新增字段的类型和数量。

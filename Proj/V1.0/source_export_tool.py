#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os
from pathlib import Path
import shutil

DEFAULT_EXCLUDES = {
    ".git", ".svn", ".hg",
    "node_modules", "build", "dist", "out", ".vscode",
    "__pycache__"
}
DEFAULT_INCLUDES = {".c", ".h", ".md"}


def parse_ext_set(csv: str) -> set[str]:
    parts = [x.strip() for x in csv.split(",") if x.strip()]
    out = set()
    for p in parts:
        if not p.startswith("."):
            p = "." + p
        out.add(p.lower())
    return out


def parse_dir_set(csv: str) -> set[str]:
    return {x.strip() for x in csv.split(",") if x.strip()}


def should_skip_dir(name: str, excludes: set[str]) -> bool:
    return name in excludes


def get_source_root() -> Path:
    # 脚本所在目录作为源目录
    return Path(__file__).resolve().parent


def export_files(src_root: Path,
                 out_root: Path,
                 includes: set[str],
                 excludes: set[str],
                 overwrite: bool,
                 verbose: bool):

    exported = 0
    skipped = 0
    errors = 0

    for root, dirs, files in os.walk(src_root):
        root_path = Path(root)

        # 防止递归进入输出目录
        if out_root == root_path or out_root in root_path.parents:
            dirs[:] = []
            continue

        dirs[:] = [d for d in dirs if not should_skip_dir(d, excludes)]

        for fn in files:
            src_file = root_path / fn
            if src_file.suffix.lower() not in includes:
                continue

            rel = src_file.relative_to(src_root)
            dst_file = (out_root / rel).with_suffix(rel.suffix + ".html")
            dst_file.parent.mkdir(parents=True, exist_ok=True)

            if dst_file.exists() and not overwrite:
                skipped += 1
                if verbose:
                    print(f"[SKIP] {rel}")
                continue

            try:
                shutil.copyfile(src_file, dst_file)
                exported += 1
                if verbose:
                    print(f"[OK] {rel} -> {dst_file.relative_to(out_root)}")
            except Exception as e:
                errors += 1
                print(f"[ERROR] {rel}: {e}")

    print(f"\nExport finished.")
    print(f"Exported: {exported}, Skipped: {skipped}, Errors: {errors}")
    print(f"Output dir: {out_root}")


def restore_inplace(out_root: Path,
                    includes: set[str],
                    overwrite: bool,
                    verbose: bool):

    restored = 0
    suffixes = [ext + ".html" for ext in includes]

    for root, _, files in os.walk(out_root):
        root_path = Path(root)

        for fn in files:
            p = root_path / fn
            lower = p.name.lower()

            for suf in suffixes:
                if lower.endswith(suf):
                    target = Path(str(p)[:-5])

                    if target.exists() and not overwrite:
                        continue
                    if target.exists():
                        target.unlink()

                    p.rename(target)
                    restored += 1
                    if verbose:
                        print(f"[RESTORE] {p.relative_to(out_root)}")
                    break

    print(f"\nRestore finished. Total: {restored}")


def restore_to_dir(out_root: Path,
                   restore_root: Path,
                   includes: set[str],
                   overwrite: bool,
                   verbose: bool):

    restored = 0
    suffixes = [ext + ".html" for ext in includes]

    for root, _, files in os.walk(out_root):
        root_path = Path(root)

        for fn in files:
            p = root_path / fn
            lower = p.name.lower()

            for suf in suffixes:
                if lower.endswith(suf):
                    rel = p.relative_to(out_root)
                    restored_rel = Path(str(rel)[:-5])
                    dst = restore_root / restored_rel
                    dst.parent.mkdir(parents=True, exist_ok=True)

                    if dst.exists() and not overwrite:
                        continue

                    shutil.copyfile(p, dst)
                    restored += 1
                    if verbose:
                        print(f"[RESTORE-TO] {rel}")
                    break

    print(f"\nRestore-to finished. Total: {restored}")
    print(f"Restore dir: {restore_root}")


def main():
    parser = argparse.ArgumentParser(
        description="Mirror .c/.h/.md from script directory to specified output directory."
    )

    parser.add_argument("--out", required=True,
                        help="Output directory (must be specified).")

    parser.add_argument("--include",
                        default=",".join(DEFAULT_INCLUDES))
    parser.add_argument("--exclude",
                        default=",".join(DEFAULT_EXCLUDES))

    parser.add_argument("--overwrite", action="store_true")
    parser.add_argument("--verbose", action="store_true")

    parser.add_argument("--restore", action="store_true")
    parser.add_argument("--restore-to", default=None)
    parser.add_argument("--restore-overwrite", action="store_true")

    args = parser.parse_args()

    src_root = get_source_root()
    out_root = Path(args.out).resolve()

    includes = parse_ext_set(args.include)
    excludes = parse_dir_set(args.exclude)

    if args.restore_to:
        restore_root = Path(args.restore_to).resolve()
        restore_root.mkdir(parents=True, exist_ok=True)
        restore_to_dir(out_root, restore_root,
                       includes,
                       args.restore_overwrite,
                       args.verbose)
        return

    if args.restore:
        restore_inplace(out_root,
                        includes,
                        args.restore_overwrite,
                        args.verbose)
        return

    out_root.mkdir(parents=True, exist_ok=True)

    print(f"Source dir: {src_root}")
    export_files(src_root,
                 out_root,
                 includes,
                 excludes,
                 args.overwrite,
                 args.verbose)


if __name__ == "__main__":
    main()
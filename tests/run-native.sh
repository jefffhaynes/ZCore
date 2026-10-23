#!/usr/bin/env bash
set -euo pipefail

REPO=$(cd "$(dirname "$0")/.." && pwd)
WORK=${ZCORE_NATIVE_DIR:-/tmp/zcore-native}
ZEPHYR_SOURCE=${ZEPHYR_SOURCE:-/mnt/c/zephyr/zephyr}
ZEPHYR_VERSION=${ZEPHYR_VERSION:-v4.3.0}

if [[ "$WORK" != /* ]]; then
    echo "ZCORE_NATIVE_DIR must be an absolute Linux path, not $WORK: run this from PowerShell, not Git Bash" >&2
    exit 1
fi

mkdir -p "$WORK"

if [ ! -d "$WORK/zephyr" ]; then
    source=https://github.com/zephyrproject-rtos/zephyr
    [ -d "$ZEPHYR_SOURCE" ] && source=file://$ZEPHYR_SOURCE
    echo "Cloning Zephyr $ZEPHYR_VERSION from $source"
    git -c core.autocrlf=false -c advice.detachedHead=false \
        clone -q --depth 1 --branch "$ZEPHYR_VERSION" "$source" "$WORK/zephyr"
fi

if [ ! -d "$WORK/py" ]; then
    echo "Installing cmake, ninja and Zephyr's Python deps"
    pip3 install -q --target "$WORK/py" "cmake>=3.20,<4" ninja -r "$WORK/zephyr/scripts/requirements-base.txt"
fi

export PYTHONPATH=$WORK/py PATH=$WORK/py/bin:$PATH ZEPHYR_BASE=$WORK/zephyr ZEPHYR_TOOLCHAIN_VARIANT=host

[ -d "$WORK/.west" ] || (cd "$WORK" && west init -l zephyr > /dev/null)

tests=("$@")
if [ ${#tests[@]} -eq 0 ]; then
    for dir in "$REPO"/tests/*/; do
        [ -f "$dir/CMakeLists.txt" ] && tests+=("$(basename "$dir")")
    done
fi

mkdir -p "$WORK/build"
failed=()
for test in "${tests[@]}"; do
    build=$WORK/build/$test
    echo "== $test"

    rm -rf "$build"

    if ! cmake -GNinja -S "$REPO/tests/$test" -B "$build" -DBOARD=native_sim/native/64 \
            -DUSER_CACHE_DIR="$WORK/cache" > "$build.configure.log" 2>&1; then
        tail -40 "$build.configure.log"; failed+=("$test"); continue
    fi

    if ! ninja -C "$build" > "$build.build.log" 2>&1; then
        { grep -E 'error' -A3 "$build.build.log" || tail -40 "$build.build.log"; } | head -60 || true
        failed+=("$test"); continue
    fi

    "$build/zephyr/zephyr.exe" -stop_at=60 | tee "$build.run.log" || true
    grep -q "PROJECT EXECUTION SUCCESSFUL" "$build.run.log" || failed+=("$test")
done

if [ ${#failed[@]} -ne 0 ]; then
    echo "FAILED: ${failed[*]}"
    exit 1
fi

echo "All passed: ${tests[*]}"

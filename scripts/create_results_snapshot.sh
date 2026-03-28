#!/usr/bin/env bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: scripts/create_results_snapshot.sh <slug> [options]

Create a versioned benchmark snapshot scaffold and capture fresh benchmark and
collision-study outputs.

Options:
  --profile=<smoke|default|deep>   Measurement profile to run (default: default)
  --date=<YYYY-MM-DD>              Snapshot date to use (default: today)
  --build-dir=<path>               CMake build directory (default: build)
  --results-dir=<path>             Output directory for markdown and artifacts
                                   (default: docs/results)
  --help                           Show this help text

The script writes:
  <results-dir>/<date>-<slug>.md
  <results-dir>/artifacts/<date>-<slug>-benchmark.txt
  <results-dir>/artifacts/<date>-<slug>-collision.txt
EOF
}

require_file() {
    local path="$1"
    if [[ ! -f "$path" ]]; then
        printf 'Missing required file: %s\n' "$path" >&2
        exit 1
    fi
}

require_command() {
    local name="$1"
    if ! command -v "$name" >/dev/null 2>&1; then
        printf 'Required command not found: %s\n' "$name" >&2
        exit 1
    fi
}

resolve_path() {
    local repo_root_path="$1"
    local candidate="$2"

    if [[ "$candidate" = /* ]]; then
        printf '%s\n' "$candidate"
    else
        printf '%s/%s\n' "$repo_root_path" "$candidate"
    fi
}

slug=''
profile='default'
snapshot_date="$(date +%F)"
build_dir='build'
results_dir='docs/results'

while (($# > 0)); do
    case "$1" in
        --profile=*)
            profile="${1#*=}"
            ;;
        --date=*)
            snapshot_date="${1#*=}"
            ;;
        --build-dir=*)
            build_dir="${1#*=}"
            ;;
        --results-dir=*)
            results_dir="${1#*=}"
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        --*)
            printf 'Unknown option: %s\n' "$1" >&2
            usage >&2
            exit 1
            ;;
        *)
            if [[ -n "$slug" ]]; then
                printf 'Unexpected positional argument: %s\n' "$1" >&2
                usage >&2
                exit 1
            fi
            slug="$1"
            ;;
    esac
    shift
done

if [[ -z "$slug" ]]; then
    printf 'Missing required snapshot slug.\n' >&2
    usage >&2
    exit 1
fi

case "$profile" in
    smoke|default|deep)
        ;;
    *)
        printf 'Invalid profile: %s\n' "$profile" >&2
        exit 1
        ;;
esac

if [[ ! "$slug" =~ ^[a-z0-9-]+$ ]]; then
    printf 'Snapshot slug must match ^[a-z0-9-]+$: %s\n' "$slug" >&2
    exit 1
fi

if [[ ! "$snapshot_date" =~ ^[0-9]{4}-[0-9]{2}-[0-9]{2}$ ]]; then
    printf 'Snapshot date must match YYYY-MM-DD: %s\n' "$snapshot_date" >&2
    exit 1
fi

repo_root="$(cd "$(dirname "$0")/.." && pwd)"
template_path="$repo_root/scripts/templates/results_snapshot.md.in"
build_dir_path="$(resolve_path "$repo_root" "$build_dir")"
results_dir_path="$(resolve_path "$repo_root" "$results_dir")"
artifact_dir_path="$results_dir_path/artifacts"
snapshot_filename="${snapshot_date}-${slug}.md"
snapshot_path="$results_dir_path/$snapshot_filename"
benchmark_artifact_path="$artifact_dir_path/${snapshot_date}-${slug}-benchmark.txt"
collision_artifact_path="$artifact_dir_path/${snapshot_date}-${slug}-collision.txt"

require_command cmake
require_file "$template_path"
require_file "$build_dir_path/CMakeCache.txt"

mkdir -p "$artifact_dir_path"

if [[ -e "$snapshot_path" ]]; then
    printf 'Snapshot already exists: %s\n' "$snapshot_path" >&2
    exit 1
fi

printf 'Building measurement tools in %s\n' "$build_dir_path"
cmake --build "$build_dir_path" --target fae_benchmark fae_collision_study

printf 'Running fae_benchmark --profile=%s\n' "$profile"
"$build_dir_path/fae_benchmark" --profile="$profile" >"$benchmark_artifact_path"

printf 'Running fae_collision_study --profile=%s\n' "$profile"
"$build_dir_path/fae_collision_study" --profile="$profile" >"$collision_artifact_path"

compiler_path="$(sed -n 's/^CMAKE_CXX_COMPILER:FILEPATH=//p' "$build_dir_path/CMakeCache.txt" | head -n 1)"
compiler_family='UnknownCompiler'
if [[ -n "$compiler_path" && -x "$compiler_path" ]]; then
    compiler_family="$("$compiler_path" --version | head -n 1)"
fi

platform="$(uname -srm)"
title_slug="${slug//-/ }"

build_flow=$(cat <<EOF
cmake --build $build_dir --target fae_benchmark fae_collision_study
./$build_dir/fae_benchmark --profile=$profile
./$build_dir/fae_collision_study --profile=$profile
EOF
)

benchmark_artifact_rel="${results_dir}/artifacts/${snapshot_date}-${slug}-benchmark.txt"
collision_artifact_rel="${results_dir}/artifacts/${snapshot_date}-${slug}-collision.txt"

sed \
    -e "s|@DATE@|$snapshot_date|g" \
    -e "s|@TITLE@|$title_slug|g" \
    -e "s|@PROFILE@|$profile|g" \
    -e "s|@PLATFORM@|$platform|g" \
    -e "s|@COMPILER@|$compiler_family|g" \
    -e "s|@BUILD_DIR@|$build_dir|g" \
    -e "s|@BENCHMARK_ARTIFACT@|$benchmark_artifact_rel|g" \
    -e "s|@COLLISION_ARTIFACT@|$collision_artifact_rel|g" \
    -e "/@BUILD_FLOW@/{
r /dev/stdin
d
}" \
    "$template_path" <<<"$build_flow" >"$snapshot_path"

printf 'Created snapshot scaffold: %s\n' "$snapshot_path"
printf 'Captured benchmark output: %s\n' "$benchmark_artifact_path"
printf 'Captured collision output: %s\n' "$collision_artifact_path"
printf 'Review the scaffold, condense the findings, then update docs/benchmark-baseline.md and docs/results/index.md if this becomes the new checked-in baseline.\n'

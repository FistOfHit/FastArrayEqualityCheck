#!/usr/bin/env bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: scripts/promote_results_snapshot.sh <snapshot-file> [options]

Promote a versioned results snapshot into the stable baseline entry points.

Arguments:
  <snapshot-file>                  Snapshot markdown file under docs/results/

Options:
  --benchmark-baseline=<path>      Benchmark baseline page to update
                                   (default: docs/benchmark-baseline.md)
  --results-index=<path>           Results archive index page to update
                                   (default: docs/results/index.md)
  --help                           Show this help text

The script updates:
  - the "Current baseline" link in docs/benchmark-baseline.md
  - the "Current snapshot" link in docs/results/index.md
  - the "All snapshots" list in docs/results/index.md
EOF
}

require_file() {
    local path="$1"
    if [[ ! -f "$path" ]]; then
        printf 'Missing required file: %s\n' "$path" >&2
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

replace_marked_block() {
    local file_path="$1"
    local marker_name="$2"
    local replacement_line="$3"

    MARKER_NAME="$marker_name" REPLACEMENT_LINE="$replacement_line" \
        perl -0pi -e '
            my $begin = "<!-- BEGIN $ENV{MARKER_NAME} -->\n";
            my $end = "\n<!-- END $ENV{MARKER_NAME} -->";
            my $replacement = $begin . $ENV{REPLACEMENT_LINE} . $end;
            s/\Q$begin\E.*?\Q$end\E/$replacement/s
                or die "Failed to replace marker block $ENV{MARKER_NAME}\n";
        ' "$file_path"
}

insert_snapshot_list_entry() {
    local file_path="$1"
    local entry_line="$2"
    local entry_target="$3"

    SNAPSHOT_ENTRY="$entry_line" SNAPSHOT_TARGET="$entry_target" perl -0pi -e '
        my $begin = "<!-- BEGIN SNAPSHOT_LIST -->\n";
        my $end = "\n<!-- END SNAPSHOT_LIST -->";
        /\Q$begin\E(.*?)\Q$end\E/s or die "Failed to find SNAPSHOT_LIST block\n";
        my @lines = grep { length } split /\n/, $1;
        my %seen;
        my @rest;
        for my $line (@lines) {
            next if $seen{$line}++;
            if ($line =~ /\(([^)]+)\)$/ && $1 eq $ENV{SNAPSHOT_TARGET}) {
                next;
            }
            push @rest, $line;
        }
        my $replacement = $begin . $ENV{SNAPSHOT_ENTRY};
        if (@rest) {
            $replacement .= "\n" . join("\n", @rest);
        }
        $replacement .= $end;
        s/\Q$begin\E.*?\Q$end\E/$replacement/s
            or die "Failed to update SNAPSHOT_LIST block\n";
    ' "$file_path"
}

snapshot=''
benchmark_baseline='docs/benchmark-baseline.md'
results_index='docs/results/index.md'

while (($# > 0)); do
    case "$1" in
        --benchmark-baseline=*)
            benchmark_baseline="${1#*=}"
            ;;
        --results-index=*)
            results_index="${1#*=}"
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
            if [[ -n "$snapshot" ]]; then
                printf 'Unexpected positional argument: %s\n' "$1" >&2
                usage >&2
                exit 1
            fi
            snapshot="$1"
            ;;
    esac
    shift
done

if [[ -z "$snapshot" ]]; then
    printf 'Missing required snapshot file.\n' >&2
    usage >&2
    exit 1
fi

repo_root="$(cd "$(dirname "$0")/.." && pwd)"
snapshot_path="$(resolve_path "$repo_root" "$snapshot")"
benchmark_baseline_path="$(resolve_path "$repo_root" "$benchmark_baseline")"
results_index_path="$(resolve_path "$repo_root" "$results_index")"

require_file "$snapshot_path"
require_file "$benchmark_baseline_path"
require_file "$results_index_path"

results_dir_path="$repo_root/docs/results"
case "$snapshot_path" in
    "$results_dir_path"/*.md)
        ;;
    *)
        printf 'Snapshot must live under docs/results/: %s\n' "$snapshot_path" >&2
        exit 1
        ;;
esac

snapshot_filename="$(basename "$snapshot_path")"
if [[ "$snapshot_filename" == "index.md" ]]; then
    printf 'Snapshot file cannot be docs/results/index.md\n' >&2
    exit 1
fi

snapshot_title="$(sed -n 's/^# //p' "$snapshot_path" | head -n 1)"
if [[ -z "$snapshot_title" ]]; then
    printf 'Snapshot file must start with a level-1 markdown heading: %s\n' "$snapshot_path" >&2
    exit 1
fi

baseline_entry="- [$snapshot_title](results/$snapshot_filename)"
archive_entry="- [$snapshot_title]($snapshot_filename)"

replace_marked_block "$benchmark_baseline_path" "CURRENT_BASELINE" "$baseline_entry"
replace_marked_block "$results_index_path" "CURRENT_SNAPSHOT" "$archive_entry"
insert_snapshot_list_entry "$results_index_path" "$archive_entry" "$snapshot_filename"

printf 'Promoted snapshot: %s\n' "$snapshot_path"
printf 'Updated baseline page: %s\n' "$benchmark_baseline_path"
printf 'Updated results index: %s\n' "$results_index_path"

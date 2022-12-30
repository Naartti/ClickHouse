#!/usr/bin/env bash

CURDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=../shell_config.sh
. "$CURDIR"/../shell_config.sh

set -e

format="$CLICKHOUSE_FORMAT --oneline"

echo "SELECT any(0) = any(1)" | $format
echo "SELECT any((NULL + NULL) = 0.0001), '1', NULL + -2147483647, any(NULL), (NULL + NULL) = 1000.0001, (NULL + NULL) = ((NULL + 10.0001) = (NULL, (NULL + 0.9999) = any(inf, 0., NULL, (NULL + 1.0001) = '214748364.6')), (NULL + NULL) = (NULL + nan))" | $format | $format

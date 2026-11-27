#!/usr/bin/env bash
set -euo pipefail

if (( $# > 3 )); then
    echo "Usage: $0 [DATABASE [USERNAME [PASSWORD]]]" >&2
    exit 2
fi

if (( $# >= 1 )); then
    database=$1
else
    IFS= read -r -p "Database name: " database </dev/tty
fi
if (( $# >= 2 )); then
    username=$2
else
    IFS= read -r -p "MariaDB username: " username </dev/tty
fi
if ! [[ $database =~ ^[a-zA-Z0-9_-]{1,64}$ && $username =~ ^[a-zA-Z0-9_.-]+$ ]]; then
    echo "Use letters, digits, underscores or hyphens in names. Usernames may also contain dots. Database names may have up to 64 characters." >&2
    exit 2
fi

if (( $# == 3 )); then
    password=$3
else
    IFS= read -rs -p "New MariaDB user password: " password </dev/tty
    echo >&2
fi
if [[ -z $password ]]; then
    echo "The password must contain at least one character." >&2
    exit 2
fi

# With NO_BACKSLASH_ESCAPES, a doubled quote represents one literal quote.
quoted_password=${password//\'/\'\'}
# Database grants interpret underscores as wildcards unless escaped.
grant_database=${database//_/\\_}

mariadb --protocol=socket --default-character-set=utf8mb4 --batch --binary-mode <<SQL
SET SESSION sql_mode = 'NO_BACKSLASH_ESCAPES';
CREATE DATABASE IF NOT EXISTS \`$database\` CHARACTER SET utf8mb4;
CREATE USER IF NOT EXISTS '$username'@'localhost' IDENTIFIED BY '$quoted_password';
GRANT ALL PRIVILEGES ON \`$grant_database\`.* TO '$username'@'localhost';
SQL

printf 'Granted %s@localhost access to %s.\n' "$username" "$database"

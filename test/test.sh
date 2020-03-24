#!/usr/bin/env bash

TEST_DIR=(test/inputs/*.txt)
EXEC="./lift_sim_A"
VALGRIND="valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --error-exitcode=2"
HELGRIND="valgrind --tool=helgrind --error-exitcode=2"

compile() {
    make clean
    make
}

print_red() {
    tput setaf 1; echo -n "$1"; tput sgr0; echo "$2"
}

print_green() {
    tput setaf 2; echo -n "$1"; tput sgr0; echo "$2"
}

run() {
    local output
    output=$($1 2>&1)
    # $1 2>&1

    if [ "$?" -eq "$2" ]; then
        print_red "failed"
        echo "$output"
    else
        print_green "passed"
    fi
}

main() {
    for f in "${TEST_DIR[@]}"; do
        echo "testing $f: "
        echo -ne "\t regular  -> "
        run "$EXEC $f 0" "1"

        echo -ne "\t valgrind -> "
        run "$VALGRIND $EXEC $f 0" "2"

        echo -ne "\t helgrind -> "
        run "$HELGRIND $EXEC $f 0" "2"
    done
}

compile
echo
main "$@"

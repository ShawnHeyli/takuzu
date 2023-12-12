#!/bin/bash
# shellcheck disable=SC2181

root_path=$( cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")" || exit ; pwd -P )
takuzu="$root_path/bin/takuzu"
takuzu_debug="$root_path/bin/takuzu_debug"
log_file="/tmp/valgrind_takuzu"

normal_test=(
  "-h"
  "tests/solver/solved"
  "-g 8"
  "-g 16 -N 10"
  #"-N 10 -g 16" # Works manually but not in the script, -g is not recognized, don't know why
  "tests/solver/heuristic"
  "tests/solver/easy"
  "tests/solver/medium"
  "tests/solver/onesolution_1"
  "tests/solver/onesolution_2"
  "tests/solver/sevensolutions"
  "tests/solver/empty_4"
)

failure_tests=(
  "tests/dwqdqwczfdasf/dasdsadasz" # No file
  "tests/valid_grids/grid_00 -g 64" # Invalid combination
  "-g 0" # Invalid grid size
  "-g 6" # Invalid grid size
  "-g 214748364772391" # Bigger than INT_MAX
  "tests/solver/nosolution" # I pretty much (if not) always get a solution ?
  "tests/solver/invalid"
  "tests/solver/severalsolutions -u"
)

success_tests=()
failed_tests=()

good_valgrind=()
bad_valgrind=() 

if [ "$1" == "debug" ]; then
  for i in "${normal_test[@]}"; do
    $takuzu "$i" &> /dev/null
    if [ $? -ne 0 ]; then
      echo "- ✗ $i"
      failed_tests+=("$i")
    else
      echo "- ✓ $i"
      success_tests+=("$i")
    fi

    # Grepping directly does not work so we need to use a temporary file
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$log_file "$takuzu_debug" "$i" &> /dev/null
    val_res="$(cat $log_file | grep "LEAK SUMMARY" -A5)"
    if [ "$val_res" == "" ]; then
      good_valgrind+=("$i")
    else
      bad_valgrind+=("$i: $val_res")
    fi
  done

  for i in "${failure_tests[@]}"; do
    $takuzu "$i" &> /dev/null
    if [ $? -eq 0 ]; then
      echo "- ✗ $i"
      failed_tests+=("$i")
    else
      echo "- ✓ $i"
      success_tests+=("$i")
    fi

    # Grepping directly does not work so we need to use a temporary file
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$log_file "$takuzu_debug" "$i" &> /dev/null
    val_res="$(cat $log_file | grep "LEAK SUMMARY" -A5)"
    if [ "$val_res" == "" ]; then
      good_valgrind+=("$i")
    else
      bad_valgrind+=("$i: $val_res")
    fi
  done
else
  for i in "${normal_test[@]}"; do
    $takuzu "$i" &> /tmp/takuzu_error
    if [ $? -ne 0 ]; then
      echo "- ✗ $i"
      failed_tests+=("$i: $(cat /tmp/takuzu_error)")
    else
      echo "- ✓ $i"
      success_tests+=("$i")
    fi
  done

  for i in "${failure_tests[@]}"; do
    $takuzu "$i" &> /dev/null
    if [ $? -eq 0 ]; then
      echo "- ✗ $i"
      failed_tests+=("$i")
    else
      echo "- ✓ $i"
      success_tests+=("$i")
    fi
  done
fi

echo "Tests passed: ${#success_tests[@]}"
for i in "${failed_tests[@]}"; do
  echo "Failure: $i"
done

if [ "$1" == "debug" ]; then
  echo "Valgrind passed: ${#good_valgrind[@]}"
  for i in "${bad_valgrind[@]}"; do
    echo "- ✗ $i"
  done
fi
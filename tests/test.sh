#!/bin/bash
# shellcheck disable=SC2181

root_path=$( cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")" || exit ; pwd -P )
takuzu="$root_path/bin/takuzu"
takuzu_debug="$root_path/bin/takuzu_debug"
log_file="/tmp/valgrind_takuzu"

normal_test=(
  "-h"
  "tests/valid_grids/grid_00"
  "tests/valid_grids/grid_consistent"
  "tests/valid_grids/grid_valid"
  "-g 8"
  "-g 16 -N 10"
)

failure_tests=(
  "tests/invalid_grids/grid_invalid_1"
  "tests/invalid_grids/grid_invalid_2"
  "tests/invalid_grids/grid_invalid_3"
  "tests/valid_grids/grid_00 -g 64"
  "-g 0"
  "-g 6"
  "-g 214748364772391" # Bigger than INT_MAX
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
    $takuzu "$i" &> /dev/null
    if [ $? -ne 0 ]; then
      echo "- ✗ $i"
      failed_tests+=("$i")
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
echo "Tests failed: ${#failed_tests[@]}"

if [ "$1" == "debug" ]; then
  echo "Valgrind passed: ${#good_valgrind[@]}"
  for i in "${bad_valgrind[@]}"; do
    echo "- ✗ $i"
  done
fi
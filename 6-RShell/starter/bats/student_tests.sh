#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "client server echo test" {
    ./dsh -s -p 12345 &
    server_pid=$!
    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
echo "hello world"
EOF

    kill $server_pid
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:12345dsh4>helloworlddsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "client server pipe test" {
    ./dsh -s -p 12345 &
    server_pid=$!
    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls | grep dshlib.c
EOF

    kill $server_pid
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:12345dsh4>dshlib.cdsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "exit closes client" {
    ./dsh -s -p 12345 &
    server_pid=$!
    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
exit
EOF

    kill $server_pid
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:12345dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "stop-server closes client and server" {
    ./dsh -s -p 12345 &
    server_pid=$!
    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
stop-server
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:127.0.0.1:12345dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}
#!/bin/bash

if [ "${1}" == "all" ]; then
    for i in 1 18 19 20 31 32 33 34 35 36 37 38 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 129 130 131 132 133 134 135 136 137 138 139 140 141 142 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 147 148 149 150 151 152 153 154 155 156 157 158 159 160 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126; do
        echo "Updating ${i}..."
        cat .pioenvs/debug/firmware.bin | mosquitto_pub -h 192.168.1.2 -t mendeleev/${i}/ota -s
        sleep 10
    done
else
    echo "Updating ${1}..."
    cat .pioenvs/debug/firmware.bin | mosquitto_pub -h 192.168.1.2 -t mendeleev/${1}/ota -s
    sleep 10
fi

wasic++ -Os                                                   \
        -z stack-size=4096 -Wl,--initial-memory=65536         \
        -Wl,--max-memory=65536                                \
        -Wl,--allow-undefined-file=src.syms                   \
        -Wl,--strip-all -nostdlib                             \
        -o example.wasm example.cpp

wasm-opt -O3 example.wasm -o example.wasm
wasm-strip example.wasm

# xxd -i example.wasm > example.wasm.h
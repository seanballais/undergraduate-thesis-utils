#/usr/bin/sh

echo "Compiling RGBA Inspector..."
g++ rgba-inspector.cpp -o rgba-inspector -lsfml-graphics -lsfml-window -lsfml-system
if [ $? -eq 0 ]; then
    echo "Compilation succeeded. Running RGBA Inspector..."
    ./rgba-inspector
else
    echo "Compilation failed."
fi
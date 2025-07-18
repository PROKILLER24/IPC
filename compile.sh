

echo "Compiling..."
g++ writer.cpp -o writer -lrt
g++ reader.cpp -o reader -lrt
echo "Compilation done."

echo "Running writer..."
./writer

echo "Running reader..."
./reader

#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>
#include <filesystem>
#include <cctype>

using namespace std;
namespace fs = filesystem;

// Función para leer un archivo en un string
string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

// Función para verificar si la ruta es un directorio
bool isDirectory(const string& path) {
    return fs::is_directory(path);
}

// Función para escribir un archivo comprimido
void writeCompressedFile(const string& inputFilename, const string& outputFilename, const string& content, int compressionLevel = Z_BEST_COMPRESSION) {
    uLongf sourceLength = (uLongf)content.size();
    uLongf destLength = compressBound(sourceLength);

    vector<char> compressedData(destLength);
    int result = compress2(reinterpret_cast<Bytef*>(compressedData.data()), &destLength,
        reinterpret_cast<const Bytef*>(content.data()), sourceLength, compressionLevel);

    if (result == Z_OK) {
        ofstream outputFile(outputFilename, ios::binary);

        // Guardar metadatos del archivo
        uint8_t isFolder = 0;  // 0 indica que es un archivo
        outputFile.write(reinterpret_cast<char*>(&isFolder), sizeof(isFolder));

        uLongf nameLength = (uLongf)inputFilename.size();
        outputFile.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        outputFile.write(inputFilename.c_str(), nameLength);

        // Guardar tamaño original del contenido
        uLongf sourceSize = (uLongf)sourceLength;
        outputFile.write(reinterpret_cast<char*>(&sourceSize), sizeof(sourceSize));

        // Guardar contenido comprimido
        outputFile.write(compressedData.data(), destLength);
        cout << "Archivo comprimido como: " << outputFilename << endl;
    }
    else {
        throw runtime_error("Error comprimiendo el archivo");
    }
}

// Función para comprimir una carpeta
void compressDirectory(const string& inputDir, const string& outputFilename, int compressionLevel = Z_BEST_COMPRESSION) {
    ofstream outputFile(outputFilename, ios::binary);

    uint8_t isFolder = 1;  // 1 indica que es una carpeta
    outputFile.write(reinterpret_cast<char*>(&isFolder), sizeof(isFolder));

    uLongf dirNameLength = (uLongf)inputDir.size();
    outputFile.write(reinterpret_cast<char*>(&dirNameLength), sizeof(dirNameLength));
    outputFile.write(inputDir.c_str(), dirNameLength);

    for (const auto& entry : fs::recursive_directory_iterator(inputDir)) {
        if (!entry.is_directory()) {
            string relativePath = fs::relative(entry.path(), inputDir).string();
            string content = readFile(entry.path().string());

            uLongf sourceLength = (uLongf)content.size();
            uLongf destLength = compressBound(sourceLength);
            vector<char> compressedData(destLength);

            int result = compress2(reinterpret_cast<Bytef*>(compressedData.data()), &destLength,
                reinterpret_cast<const Bytef*>(content.data()), sourceLength, compressionLevel);

            if (result == Z_OK) {
                uLongf pathLength = (uLongf)relativePath.size();
                outputFile.write(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
                outputFile.write(relativePath.c_str(), pathLength);

                // Guardar tamaño original del contenido
                uLongf sourceSize = (uLongf)sourceLength;
                outputFile.write(reinterpret_cast<char*>(&sourceSize), sizeof(sourceSize));

                // Guardar contenido comprimido
                outputFile.write(reinterpret_cast<char*>(&destLength), sizeof(destLength));
                outputFile.write(compressedData.data(), destLength);
            }
        }
    }

    cout << "Carpeta comprimida como: " << outputFilename << endl;
}

// Función para descomprimir una carpeta
void decompressDirectory(const string& compressedFilename) {
    ifstream inputFile(compressedFilename, ios::binary);

    uint8_t isFolder;
    inputFile.read(reinterpret_cast<char*>(&isFolder), sizeof(isFolder));

    if (isFolder == 1) {
        uLongf dirNameLength;
        inputFile.read(reinterpret_cast<char*>(&dirNameLength), sizeof(dirNameLength));

        string baseFolderName(dirNameLength, '\0');
        inputFile.read(&baseFolderName[0], dirNameLength);

        fs::create_directory(baseFolderName);

        while (inputFile) {
            uLongf pathLength;
            if (!inputFile.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength))) break;

            string relativePath(pathLength, '\0');
            inputFile.read(&relativePath[0], pathLength);

            // Leer el tamaño original antes de descomprimir
            uLongf originalSize;
            inputFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));

            uLongf compressedSize;
            inputFile.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));

            vector<char> compressedData(compressedSize);
            inputFile.read(compressedData.data(), compressedSize);

            vector<char> uncompressedData(originalSize);

            int result = uncompress(reinterpret_cast<Bytef*>(uncompressedData.data()), &originalSize,
                reinterpret_cast<const Bytef*>(compressedData.data()), compressedSize);

            if (result == Z_OK) {
                fs::path fullPath = fs::path(baseFolderName) / relativePath;
                fs::create_directories(fullPath.parent_path());

                ofstream outputFile(fullPath, ios::binary);
                outputFile.write(uncompressedData.data(), originalSize);
            }
            else {
                cout << "No se pudo descomprimir el archivo: " << relativePath << "\n";
            }
        }

        cout << "Carpeta descomprimida en: " << baseFolderName << endl;
    }
    else {
        throw runtime_error("No es una carpeta comprimida");
    }
}

// Función para leer archivo comprimido y obtener su tipo
void decompressFile(const string& compressedFilename) {
    ifstream inputFile(compressedFilename, ios::binary);

    uint8_t isFolder;
    inputFile.read(reinterpret_cast<char*>(&isFolder), sizeof(isFolder));

    if (isFolder == 1) {
        decompressDirectory(compressedFilename);
    }
    else {
        string originalFilename;
        uLongf nameLength;
        inputFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        originalFilename.resize(nameLength);
        inputFile.read(&originalFilename[0], nameLength);

        // Leer el tamaño original antes de descomprimir
        uLongf originalSize;
        inputFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));

        string compressedData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());

        vector<char> uncompressedData(originalSize);

        int result = uncompress(reinterpret_cast<Bytef*>(uncompressedData.data()), &originalSize,
            reinterpret_cast<const Bytef*>(compressedData.data()), compressedData.size());

        if (result == Z_OK) {
            ofstream outputFile(originalFilename, ios::binary);
            outputFile.write(uncompressedData.data(), originalSize);
            cout << "Archivo descomprimido como: " << originalFilename << endl;
        }
        else {
            throw runtime_error("Error descomprimiendo el archivo");
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc > 2) {
        char* op = argv[1]; // 1 para comprimir, 2 para descomprimir
        switch (*op)
        {
        case '1': {
            string inputPath = argv[2];
            string baseName = fs::path(inputPath).stem().string(); //fs::path(inputPath).filename().string()
            baseName[0] = isalpha(baseName[0]) ? toupper(baseName[0]) : baseName[0];
            string outputFilename ="comprimido" + baseName + ".gzip";
            cout<<"Comprimiendo archivo..."<<endl;
            if (isDirectory(inputPath)) {
				compressDirectory(inputPath, outputFilename);
			}
			else {
				string fileData = readFile(inputPath);
				writeCompressedFile(inputPath, outputFilename, fileData);
			}
            break;
        }
        case '2': {
            cout<<"Descomprimiendo archivo..."<<endl;
			string inputPath = argv[2];
			decompressFile(inputPath);
			break;
        }
        default:
			cout << "Opción no válida\n";
			break;
		}
    }
	else {
		cout << "Faltan argumentos\n";
	}

    return 0;
}
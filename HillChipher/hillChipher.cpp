#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cmath>
#include <random>
#include <ctime>
#include <cstdlib>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BOLD    = "\033[1m";
}

class RussianAlphabet {
private:
    static const std::vector<std::string> letters;
    
public:
    static const int MOD = 33;
    
    static std::vector<int> textToNumbers(const std::string& text) {
        std::vector<int> result;
        
        for (size_t i = 0; i < text.length(); i += 2) {
            std::string rusChar = text.substr(i, 2);
            int num = charToNumber(rusChar);
            if (num != -1) {
                result.push_back(num);
            }
        }
        
        return result;
    }
    
    static std::string numbersToText(const std::vector<int>& nums) {
        std::string result;
        for (int num : nums) {
            result += numberToChar(num);
        }
        return result;
    }
    
    static int charToNumber(const std::string& ch) {
        auto it = std::find(letters.begin(), letters.end(), ch);
        if (it != letters.end()) {
            return std::distance(letters.begin(), it);
        }
        return -1;
    }
    
    static std::string numberToChar(int num) {
        num = (num % MOD + MOD) % MOD;
        return letters[num];
    }

    static void printAlphabet() {
        std::cout << "   ";
        for (int i = 0; i < MOD; ++i) {
            std::cout << letters[i] << "(" << std::setw(2) << i << ") ";
            if ((i + 1) % 11 == 0) std::cout << "\n   ";
        }
        std::cout << std::endl;
    }
};

class Matrix {
private:
    std::vector<std::vector<int>> data;
    int size;
    int mod;

public:
    Matrix(int n, int m = RussianAlphabet::MOD) : size(n), mod(m) {
        data.resize(n, std::vector<int>(n, 0));
    }

    Matrix(const std::vector<std::vector<int>>& mat, int m = RussianAlphabet::MOD) : mod(m) {
        size = mat.size();
        data = mat;
        validateMatrix();
    }

    int getSize() const { 
        return size; 
    }

    int getMod() const { 
        return mod; 
    }

    void validateMatrix() {
        normalize();
        
        if (!hasValidElements()) {
            std::cout << Color::RED << "    ПРЕДУПРЕЖДЕНИЕ: Некоторые элементы матрицы выходят за пределы [0, " 
                      << (mod - 1) << "]. Они будут приведены по модулю " << mod << "." << Color::RESET << std::endl;
            normalize();
        }
    }

    bool hasValidElements() const {
        for (int i = 0; i < size; ++i) {
            for(int j = 0; j < size; ++j) {
                if(data[i][j] < 0 || data[i][j] >= mod) {
                    return false;
                }
            }
        }
        return true;
    }

    void normalize() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                data[i][j] = ((data[i][j] % mod) + mod) % mod;
            }
        }
    }

    static int gcd(int a, int b) {
        a = std::abs(a);
        b = std::abs(b);
        while (b != 0) {
            int t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    std::vector<int>& operator[](int i) { 
        return data[i]; 
    }

    std::vector<int> operator[](int i) const { 
        return data[i]; 
    }

    int determinantRaw() const {
        if (size == 1) {
            return data[0][0];
        }
        if (size == 2) {
            return (data[0][0] * data[1][1] - data[0][1] * data[1][0]);
        }
        if (size == 3) {
            int det = data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1])
                    - data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0])
                    + data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
            return det;
        }
        if(size == 4) {
            int det = 0;
            for (int j = 0; j < 4; ++j) {
                Matrix minor = getMinor(0, j);
                int sign = (j % 2 == 0) ? 1 : -1;
                det += sign * data[0][j] * minor.determinantRaw();
            }
            return det;
        }
        
        throw std::runtime_error("Определитель для матриц размером больше 4x4 не реализован");
    }

    int determinant() const {
        int det = determinantRaw();
        return ((det % mod) + mod) % mod;
    }

    Matrix getMinor(int row, int col) const {
        Matrix minor(size - 1, mod);
        int r = 0;
        int c = 0;
        for(int i = 0; i < size; ++i) {
            if (i == row) {
                continue;
            }
            c = 0;
            for (int j = 0; j < size; ++j) {
                if (j == col) {
                    continue;
                }
                minor[r][c] = data[i][j];
                ++c;
            }
            ++r;
        }
        return minor;
    }

    Matrix multiply(const Matrix& other) const {
        Matrix result(size, mod);
        std::cout << Color::CYAN << "\n   [Умножение матриц]:" << Color::RESET << std::endl;
        
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                int sum = 0;
                std::cout << "   C[" << i << "][" << j << "] = ";

                for (int k = 0; k < size; ++k) {
                    sum += data[i][k] * other[k][j];
                    std::cout << data[i][k] << "*" << other[k][j];
                    if (k < size - 1) {
                        std::cout << " + ";
                    }
                }

                result[i][j] = ((sum % mod) + mod) % mod;
                std::cout << " = " << sum << " ≡ " << result[i][j] << " (mod " << mod << ")" << std::endl;
            }
        }
        return result;
    }

    std::vector<int> multiplyVector(const std::vector<int>& vec) const {
        std::vector<int> result(size, 0);
        std::cout << Color::CYAN << "   [Умножение матрицы на вектор]:" << Color::RESET << std::endl;
        
        for (int i = 0; i < size; ++i) {
            int sum = 0;
            std::cout << "   C[" << i << "] = ";

            for (int j = 0; j < size; ++j) {
                sum += data[i][j] * vec[j];
                std::cout << data[i][j] << "*" << vec[j];
                if (j < size - 1) {
                    std::cout << " + ";
                }
            }

            result[i] = ((sum % mod) + mod) % mod;
            std::cout << " = " << sum << " ≡ " << result[i] << " (mod " << mod << ")" << std::endl;
        }
        return result;
    }

    static int modInverse(int a, int m) {
        a = (a % m + m) % m;

        int m0 = m;
        int y = 0;
        int x = 1;

        if (m == 1) {
            return 0;
        }

        while (a > 1) {
            int q = a / m;
            int t = m;
            m = a % m;
            a = t;
            t = y;
            y = x - q * y;
            x = t;
        }

        if (x < 0) {
            x += m0;
        }
        return x;
    }

    Matrix inverse() const {
        std::cout << Color::YELLOW << "\n   [Вычисление обратной матрицы]:" << Color::RESET << std::endl;
        
        if (!isInvertible()) {
            throw std::runtime_error("Матрица необратима! Определитель = 0 или НОД(дет, модуль) != 1");
        }
        
        int det = determinant();
        int detInv = modInverse(det, mod);
        
        std::cout << "   Обратный элемент к определителю: " << detInv << std::endl;
        std::cout << "   Проверка: " << det << " * " << detInv << " = " << (det * detInv) % mod << " ≡ 1 (mod " << mod << ")" << std::endl;

        Matrix inv(size, mod);
        
        if (size == 2) {
            inv[0][0] = data[1][1];
            inv[0][1] = -data[0][1];
            inv[1][0] = -data[1][0];
            inv[1][1] = data[0][0];
            
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    inv[i][j] = ((inv[i][j] * detInv) % mod + mod) % mod;
                }
            }
        } else {
            Matrix cof(size, mod);
            
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    Matrix minor = getMinor(i, j);
                    int minorDet = minor.determinantRaw();
                    int sign = ((i + j) % 2 == 0) ? 1 : -1;
                    cof[i][j] = sign * minorDet;
                }
            }
            
            std::cout << "\n   Матрица алгебраических дополнений:" << std::endl;
            cof.print();
            
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    inv[i][j] = ((cof[j][i] * detInv) % mod + mod) % mod;
                }
            }
        }

        return inv;
    }

    bool isInvertible() const {
        int det = determinant();
        
        std::cout << Color::CYAN << "\n   [Проверка обратимости матрицы]" << Color::RESET << std::endl;
        std::cout << "   Определитель по модулю " << mod << ": " << det << std::endl;
        
        if (det == 0) {
            std::cout << Color::RED << "   ОШИБКА: Определитель равен 0. Матрица вырожденная!" << Color::RESET << std::endl;
            return false;
        }
        
        int gcd_val = Matrix::gcd(det, mod);
        std::cout << "   НОД(" << det << ", " << mod << ") = " << gcd_val << std::endl;
        
        if (gcd_val != 1) {
            std::cout << Color::RED << "   ОШИБКА: НОД(определитель, модуль) = " << gcd_val << " (должен быть 1 для обратимости)" << Color::RESET << std::endl;
            return false;
        }
        
        std::cout << Color::GREEN << "   Матрица обратима по модулю " << mod << Color::RESET << std::endl;
        return true;
    }

    void print() const {
        for (int i = 0; i < size; ++i) {
            std::cout << "   [ ";
            for (int j = 0; j < size; ++j) {
                std::cout << std::setw(4) << data[i][j] << " ";
            }
            std::cout << "]" << std::endl;
        }
    }
};

class MatrixGenerator {
public:
    static void init() {
        std::srand(std::time(nullptr));
    }
    
    static Matrix generateInvertibleMatrix(int size, int mod = RussianAlphabet::MOD) {
        std::mt19937 rng(std::random_device{}());
        
        while (true) {
            std::vector<std::vector<int>> mat(size, std::vector<int>(size));
            
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    mat[i][j] = rng() % mod;
                }
            }
            
            Matrix matrix(mat, mod);
            int det = matrix.determinant();
            
            if (det != 0 && Matrix::gcd(det, mod) == 1) {
                return matrix;
            }
        }
    }
};

class HillCipher {
private:
    int mod;
    int blockSize;

    std::vector<int> preprocessText(const std::string& text) {
        std::vector<int> numbers = RussianAlphabet::textToNumbers(text);
        
        while (numbers.size() % blockSize != 0) {
            numbers.push_back(0);
        }
        return numbers;
    }

public:
    HillCipher(int size) : blockSize(size), mod(RussianAlphabet::MOD) {}

     std::string encrypt(const std::string& plaintext, const Matrix& key) {
        std::cout << Color::BOLD << Color::GREEN << "\n    ШИФРОВАНИЕ    " << Color::RESET << std::endl;
        
        std::vector<int> numbers = preprocessText(plaintext);
        
        std::cout << "Исходный текст: " << Color::BLUE << plaintext << Color::RESET << std::endl;
        std::cout << "Числа: [ ";
        for (int num : numbers) {
            std::cout << num << " ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "Размер блока: " << blockSize << std::endl;
        std::cout << "\nКлючевая матрица:" << std::endl;
        key.print();

        std::vector<int> encryptedNumbers;
        std::cout << Color::CYAN << "\n  Процесс шифрования блоков  " << Color::RESET << std::endl;
        
        for (size_t i = 0; i < numbers.size(); i += blockSize) {
            std::vector<int> block(numbers.begin() + i, numbers.begin() + i + blockSize);
            std::cout << "\nБлок " << (i / blockSize + 1) << ": [ ";
            for (size_t j = 0; j < block.size(); ++j) {
                std::cout << block[j] << (j < block.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << std::endl;
            
            std::vector<int> encryptedBlock = key.multiplyVector(block);
            std::cout << "Результат: [ ";
            for (size_t j = 0; j < encryptedBlock.size(); ++j) {
                std::cout << encryptedBlock[j] << (j < encryptedBlock.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << std::endl;
            
            encryptedNumbers.insert(encryptedNumbers.end(), encryptedBlock.begin(), encryptedBlock.end());
        }
        
        std::string ciphertext = RussianAlphabet::numbersToText(encryptedNumbers);
        std::cout << Color::BOLD << Color::GREEN << "\nИтоговый шифротекст: " << Color::YELLOW << ciphertext << Color::RESET << std::endl;

        return ciphertext;
    }

    std::string decrypt(const std::string& ciphertext, const Matrix& key) {
        std::cout << Color::BOLD << Color::GREEN << "\n    ДЕШИФРОВАНИЕ    " << Color::RESET << std::endl;
        
        std::vector<int> numbers = RussianAlphabet::textToNumbers(ciphertext);
        
        std::cout << "Шифротекст: " << Color::BLUE << ciphertext << Color::RESET << std::endl;
        std::cout << "Числа: [ ";
        for (int num : numbers) {
            std::cout << num << " ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "\nИсходная ключевая матрица:" << std::endl;
        key.print();

        if (!key.isInvertible()) {
            throw std::runtime_error("Ключевая матрица необратима! Невозможно расшифровать.");
        }

        Matrix invKey = key.inverse();
        std::cout << "\nОбратная матрица (ключ для расшифровки):" << std::endl;
        invKey.print();

        std::vector<int> decryptedNumbers;
        std::cout << Color::CYAN << "\n  Процесс расшифрования блоков  " << Color::RESET << std::endl;

        for (size_t i = 0; i < numbers.size(); i += blockSize) {
            std::vector<int> block(numbers.begin() + i, numbers.begin() + i + blockSize);
            std::cout << "\nБлок " << (i / blockSize + 1) << ": [ ";
            for (size_t j = 0; j < block.size(); ++j) {
                std::cout << block[j] << (j < block.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << std::endl;
            
            std::vector<int> decryptedBlock = invKey.multiplyVector(block);
            std::cout << "Результат: [ ";
            for (size_t j = 0; j < decryptedBlock.size(); ++j) {
                std::cout << decryptedBlock[j] << (j < decryptedBlock.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << std::endl;
            
            decryptedNumbers.insert(decryptedNumbers.end(), decryptedBlock.begin(), decryptedBlock.end());
        }
        
        std::string plaintext = RussianAlphabet::numbersToText(decryptedNumbers);
        std::cout << Color::BOLD << Color::GREEN << "\nИтоговый открытый текст: " << Color::YELLOW << plaintext << Color::RESET << std::endl;
        return plaintext;
    }

    std::string encryptRecurrent(const std::string& plaintext, const Matrix& K0, const Matrix& K1) {
        std::cout << Color::BOLD << Color::MAGENTA << "\n    РЕКУРРЕНТНОЕ ШИФРОВАНИЕ    " << Color::RESET << std::endl;
        std::cout << Color::YELLOW << "Формула: K_i = K_{i-1} * K_{i-2}" << Color::RESET << std::endl;
        
        std::vector<int> numbers = preprocessText(plaintext);
        int numBlocks = numbers.size() / blockSize;
        
        std::cout << "Исходный текст: " << Color::BLUE << plaintext << Color::RESET << std::endl;
        std::cout << "Числа: [ ";
        for (int num : numbers) {
            std::cout << num << " ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "\nНачальные матрицы:" << std::endl;
        std::cout << "K0:" << std::endl;
        K0.print();
        std::cout << "K1:" << std::endl;
        K1.print();
        std::vector<Matrix> keys;
        keys.push_back(K0);
        keys.push_back(K1);
        
        std::cout << Color::CYAN << "\n  Генерация ключей " << Color::RESET << std::endl;
        for (int i = 2; i <= numBlocks; ++i) {
            Matrix K_new = keys[i-1].multiply(keys[i-2]);
            keys.push_back(K_new);
            std::cout << "K" << i << " = K" << (i-1) << " * K" << (i-2) << ":" << std::endl;
            K_new.print();
        }

        std::vector<int> encryptedNumbers;
        std::cout << Color::CYAN << "\n  Шифрование блоков " << Color::RESET << std::endl;
        
        for (int i = 0; i < numBlocks; ++i) {
            std::cout << Color::YELLOW << "\n  Блок " << (i + 1) << " (ключ K" << i << ")    " << Color::RESET << std::endl;
            
            std::vector<int> block(numbers.begin() + i * blockSize, numbers.begin() + (i + 1) * blockSize);
            
            std::cout << "Открытый текст: [ ";
            for (size_t j = 0; j < block.size(); ++j) {
                std::cout << block[j] << (j < block.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << std::endl;
            
            std::vector<int> encryptedBlock = keys[i].multiplyVector(block);
            
            std::cout << "Шифротекст: [ ";
            for (size_t j = 0; j < encryptedBlock.size(); ++j) {
                std::cout << encryptedBlock[j] << (j < encryptedBlock.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << Color::RESET << std::endl;
            
            encryptedNumbers.insert(encryptedNumbers.end(), encryptedBlock.begin(), encryptedBlock.end());
        }
        
        std::string ciphertext = RussianAlphabet::numbersToText(encryptedNumbers);
        std::cout << Color::BOLD << Color::MAGENTA << "\nИтоговый шифротекст: " << Color::YELLOW << ciphertext << Color::RESET << std::endl;
        return ciphertext;
    }

    std::string decryptRecurrent(const std::string& ciphertext, const Matrix& K0, const Matrix& K1) {
        std::cout << Color::BOLD << Color::MAGENTA << "\n    РЕКУРРЕНТНОЕ ДЕШИФРОВАНИЕ    " << Color::RESET << std::endl;
        std::cout << Color::YELLOW << "Формула: K_i = K_{i-1} * K_{i-2}" << Color::RESET << std::endl;
        
        std::vector<int> numbers = RussianAlphabet::textToNumbers(ciphertext);
        int numBlocks = numbers.size() / blockSize;
        
        std::cout << "Шифротекст: " << Color::BLUE << ciphertext << Color::RESET << std::endl;
        std::cout << "Числа: [ ";
        for (int num : numbers) {
            std::cout << num << " ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "\nНачальные матрицы:" << std::endl;
        std::cout << "K0:" << std::endl;
        K0.print();
        std::cout << "K1:" << std::endl;
        K1.print();

        std::vector<Matrix> keys;
        keys.push_back(K0);
        keys.push_back(K1);
        
        std::cout << Color::CYAN << "\n  Генерация ключей  " << Color::RESET << std::endl;
        for (int i = 2; i <= numBlocks; ++i) {
            Matrix K_new = keys[i-1].multiply(keys[i-2]);
            keys.push_back(K_new);
            std::cout << "K" << i << " = K" << (i-1) << " * K" << (i-2) << ":" << std::endl;
            K_new.print();
        }

        std::vector<int> decryptedNumbers;
        std::cout << Color::CYAN << "\n  Расшифрование блоков  " << Color::RESET << std::endl;
        
        for (int i = 0; i < numBlocks; ++i) {
            std::cout << Color::YELLOW << "\n  Блок " << (i + 1) << " (ключ K" << i << "^-1)  " << Color::RESET << std::endl;
            Matrix invKey = keys[i].inverse();
            
            std::vector<int> block(numbers.begin() + i * blockSize, numbers.begin() + (i + 1) * blockSize);
            
            std::cout << "Шифротекст: [ ";
            for (size_t j = 0; j < block.size(); ++j) {
                std::cout << block[j] << (j < block.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << std::endl;
            
            std::vector<int> decryptedBlock = invKey.multiplyVector(block);
            
            std::cout << "Открытый текст: [ ";
            for (size_t j = 0; j < decryptedBlock.size(); ++j) {
                std::cout << decryptedBlock[j] << (j < decryptedBlock.size() - 1 ? ", " : "");
            }
            std::cout << " ]" << Color::RESET << std::endl;
            
            decryptedNumbers.insert(decryptedNumbers.end(), decryptedBlock.begin(), decryptedBlock.end());
        }
        
        std::string plaintext = RussianAlphabet::numbersToText(decryptedNumbers);
        std::cout << Color::BOLD << Color::MAGENTA << "\nИтоговый открытый текст: " << Color::YELLOW << plaintext << Color::RESET << std::endl;
        return plaintext;
    }
};

const std::vector<std::string> RussianAlphabet::letters = {
    "а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й",
    "к", "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф",
    "х", "ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"
};

class UserInterface {
private:
    void clearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void printHeader() {
        std::cout << R"(
  ╔═══════════════════════════════════════════════════════════════════════╗
  ║                    ШИФР ХИЛЛА И РЕКУРРЕНТНЫЙ ШИФР ХИЛЛА               ║
  ║                             (русский алфавит)                         ║
  ╚═══════════════════════════════════════════════════════════════════════╝
        )" << Color::RESET << std::endl;
    }

    void printMenu() {
        std::cout << Color::BOLD << Color::CYAN << "\n  ╔════════════════════════════════════╗" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ║          ГЛАВНОЕ МЕНЮ              ║" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ╠════════════════════════════════════╣" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ║ " << Color::YELLOW << "1." << Color::WHITE << " Классический шифр Хилла         " << Color::CYAN << "║" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ║ " << Color::YELLOW << "2." << Color::WHITE << " Рекуррентный шифр Хилла         " << Color::CYAN << "║" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ║ " << Color::YELLOW << "3." << Color::WHITE << " Показать алфавит                " << Color::CYAN << "║" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ║ " << Color::YELLOW << "4." << Color::WHITE << " Справка                         " << Color::CYAN << "║" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ║ " << Color::YELLOW << "0." << Color::WHITE << " Выход                           " << Color::CYAN << "║" << Color::RESET;
        std::cout << Color::BOLD << Color::CYAN << "\n  ╚════════════════════════════════════╝" << Color::RESET;
        std::cout << Color::YELLOW << "\n\n  Ваш выбор: " << Color::RESET;
    }

    int getMatrixSize() {
        int size;
        while (true) {
            std::cout << Color::CYAN << "  Введите размер матрицы (2, 3 или 4): " << Color::RESET;
            std::cin >> size;
            if (std::cin.fail()) {
                clearInputBuffer();
                std::cout << Color::RED << "    Ошибка! Введите число.\n" << Color::RESET;
                continue;
            }
            if (size >= 2 && size <= 4) {
                clearInputBuffer();
                return size;
            }
            std::cout << Color::RED << "    Ошибка! Размер должен быть 2, 3 или 4.\n" << Color::RESET;
        }
    }

    Matrix inputMatrix(int size, const std::string& name = "") {
        while (true) {
            std::cout << Color::CYAN << "\n  Введите элементы матрицы " << name << size << "x" << size << " (числа от 0 до 32):" << Color::RESET << std::endl;
            std::cout << Color::YELLOW << "  Вводите построчно, разделяя пробелами:" << Color::RESET << std::endl;
            
            std::vector<std::vector<int>> mat(size, std::vector<int>(size));
            bool validInput = true;
            
            clearInputBuffer();
            
            for (int i = 0; i < size && validInput; ++i) {
                std::cout << "  Строка " << (i + 1) << ": ";
                std::string line;
                std::getline(std::cin, line);
                
                std::istringstream iss(line);
                for (int j = 0; j < size; ++j) {
                    if (!(iss >> mat[i][j])) {
                        std::cout << Color::RED << "    Ошибка ввода! Недостаточно чисел в строке." << Color::RESET << std::endl;
                        validInput = false;
                        break;
                    }
                }
            }
            
            if (!validInput) {
                std::cout << Color::RED << "  Попробуйте ввести матрицу заново.\n" << Color::RESET << std::endl;
                continue;
            }
            
            Matrix matrix(mat);
            
            std::cout << Color::GREEN << "\n  Введённая матрица:" << Color::RESET << std::endl;
            matrix.print();
            
            int det = matrix.determinant();
            int gcd_val = Matrix::gcd(det, RussianAlphabet::MOD);
            
            std::cout << "\n  Определитель по модулю 33: " << det << std::endl;
            std::cout << "  НОД(" << det << ", 33) = " << gcd_val << std::endl;
            
            if (det == 0) {
                std::cout << Color::RED << "\n    ОШИБКА: Определитель равен 0!" << Color::RESET << std::endl;
                continue;
            }
            
            if (gcd_val != 1) {
                std::cout << Color::RED << "\n    ОШИБКА: НОД(определитель, 33) = " << gcd_val << " ≠ 1" << Color::RESET << std::endl;
                continue;
            }
            
            std::cout << Color::GREEN << "\n     МАТРИЦА КОРРЕКТНА!" << Color::RESET << std::endl;
            std::cout << Color::CYAN << "\n  Использовать эту матрицу? (что-угодно/n): " << Color::RESET;
            
            char choice;
            std::cin >> choice;
            clearInputBuffer();
            
              if (choice != 'n' && choice != 'N') {
                return matrix;
            }
        }
    }

    std::string inputText(const std::string& prompt) {
        while (true) {
            std::cout << Color::CYAN << "  " << prompt << Color::RESET;
            std::string text;
            std::getline(std::cin, text);
            
            if (text.empty()) {
                std::cout << Color::RED << "    Текст не может быть пустым!" << Color::RESET << std::endl;
                continue;
            }
            
            std::string processedText;
            for (size_t i = 0; i < text.length(); ) {
                unsigned char c = text[i];
                if (std::isspace(c)) {
                    i++;
                    continue;
                }
                if (c < 128) {
                    i++;
                    continue;
                }
                if ((c & 0xE0) == 0xC0 && i + 1 < text.length()) {
                    std::string rusChar = text.substr(i, 2);
                    if (rusChar == "А" || rusChar == "а") rusChar = "а";
                    else if (rusChar == "Б" || rusChar == "б") rusChar = "б";
                    else if (rusChar == "В" || rusChar == "в") rusChar = "в";
                    else if (rusChar == "Г" || rusChar == "г") rusChar = "г";
                    else if (rusChar == "Д" || rusChar == "д") rusChar = "д";
                    else if (rusChar == "Е" || rusChar == "е") rusChar = "е";
                    else if (rusChar == "Ё" || rusChar == "ё") rusChar = "ё";
                    else if (rusChar == "Ж" || rusChar == "ж") rusChar = "ж";
                    else if (rusChar == "З" || rusChar == "з") rusChar = "з";
                    else if (rusChar == "И" || rusChar == "и") rusChar = "и";
                    else if (rusChar == "Й" || rusChar == "й") rusChar = "й";
                    else if (rusChar == "К" || rusChar == "к") rusChar = "к";
                    else if (rusChar == "Л" || rusChar == "л") rusChar = "л";
                    else if (rusChar == "М" || rusChar == "м") rusChar = "м";
                    else if (rusChar == "Н" || rusChar == "н") rusChar = "н";
                    else if (rusChar == "О" || rusChar == "о") rusChar = "о";
                    else if (rusChar == "П" || rusChar == "п") rusChar = "п";
                    else if (rusChar == "Р" || rusChar == "р") rusChar = "р";
                    else if (rusChar == "С" || rusChar == "с") rusChar = "с";
                    else if (rusChar == "Т" || rusChar == "т") rusChar = "т";
                    else if (rusChar == "У" || rusChar == "у") rusChar = "у";
                    else if (rusChar == "Ф" || rusChar == "ф") rusChar = "ф";
                    else if (rusChar == "Х" || rusChar == "х") rusChar = "х";
                    else if (rusChar == "Ц" || rusChar == "ц") rusChar = "ц";
                    else if (rusChar == "Ч" || rusChar == "ч") rusChar = "ч";
                    else if (rusChar == "Ш" || rusChar == "ш") rusChar = "ш";
                    else if (rusChar == "Щ" || rusChar == "щ") rusChar = "щ";
                    else if (rusChar == "Ъ" || rusChar == "ъ") rusChar = "ъ";
                    else if (rusChar == "Ы" || rusChar == "ы") rusChar = "ы";
                    else if (rusChar == "Ь" || rusChar == "ь") rusChar = "ь";
                    else if (rusChar == "Э" || rusChar == "э") rusChar = "э";
                    else if (rusChar == "Ю" || rusChar == "ю") rusChar = "ю";
                    else if (rusChar == "Я" || rusChar == "я") rusChar = "я";
                    processedText += rusChar;
                    i += 2;
                } else {
                    i++;
                }
            }
            
            std::vector<int> numbers = RussianAlphabet::textToNumbers(processedText);
            
            if (numbers.empty()) {
                std::cout << Color::RED << "    Текст не содержит букв русского алфавита!" << Color::RESET << std::endl;
                continue;
            }
            
            std::cout << Color::GREEN << "  Обработанный текст (без пробелов): " << Color::YELLOW;
            for (int num : numbers) {
                std::cout << RussianAlphabet::numberToChar(num);
            }
            std::cout << Color::RESET << std::endl;
            std::cout << Color::CYAN << "\n  Продолжить? (что-угодно/n): " << Color::RESET;
            char choice;
            std::cin >> choice;
            clearInputBuffer();
            
              if (choice != 'n' && choice != 'N' ) {
                return processedText;
            }
        }
    }

    void showAlphabet() {
        RussianAlphabet::printAlphabet();
    }

    void showHelp() {
        std::cout << Color::WHITE << R"(
  1. Классический шифр Хилла:
     - Использует одну матрицу-ключ для всех блоков
     - Матрица должна быть обратима по модулю 33

  2. Рекуррентный шифр Хилла:
     - Требует две начальные матрицы K0 и K1
     - Формула обновления: K_i = K_{i-1} * K_{i-2}
     - Обе матрицы должны быть обратимы

  3. Алфавит:
     - 33 буквы русского алфавита (включая 'ё')
     - Нумерация: а=0, б=1, ..., я=32

  4. Автоматическая генерация:
     - Можно сгенерировать обратимые матрицы автоматически
     - Генератор создаёт матрицы с НОД(дет, 33) = 1
        )" << Color::RESET << std::endl;
    }

    void pressEnterToContinue() {
        std::cout << Color::YELLOW << "\n  Нажмите Enter для продолжения..." << Color::RESET;
        std::cin.get();
    }

public:
    void run() {
        #ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
        #else
            setlocale(LC_ALL, "en_US.UTF-8");
        #endif
        
        MatrixGenerator::init();
        printHeader();
        
        while (true) {
            printMenu();
            
            int choice;
            std::cin >> choice;
            clearInputBuffer();
            
            if (choice == 0) {
                std::cout << Color::GREEN << "\n  До свидания!\n" << Color::RESET << std::endl;
                break;
            }
            
            try {
                switch (choice) {
                    case 1: {
                        std::cout << Color::BOLD << Color::GREEN << "\n  ╔════════════════════════════════════════╗" << Color::RESET;
                        std::cout << Color::BOLD << Color::GREEN << "\n  ║       КЛАССИЧЕСКИЙ ШИФР ХИЛЛА          ║" << Color::RESET;
                        std::cout << Color::BOLD << Color::GREEN << "\n  ╚════════════════════════════════════════╝" << Color::RESET << std::endl;
                        
                        int size = getMatrixSize();
                        
                        std::cout << Color::CYAN << "\n  Выберите способ ввода матрицы:" << Color::RESET << std::endl;
                        std::cout << "    1. Ввести вручную" << std::endl;
                        std::cout << "    2. Сгенерировать автоматически" << std::endl;
                        std::cout << "  Выбор: ";
                        
                        int inputChoice;
                        std::cin >> inputChoice;
                        clearInputBuffer();
                        
                        Matrix key(size);
                        if (inputChoice == 1) {
                            key = inputMatrix(size);
                        } else {
                            key = MatrixGenerator::generateInvertibleMatrix(size);
                            std::cout << Color::GREEN << "\n  Сгенерированная матрица:" << Color::RESET << std::endl;
                            key.print();
                        }
                        
                        std::cout << Color::CYAN << "\n  Выберите операцию:" << Color::RESET << std::endl;
                        std::cout << "    1. Зашифровать" << std::endl;
                        std::cout << "    2. Расшифровать" << std::endl;
                        std::cout << "  Выбор: ";
                        
                        int op;
                        std::cin >> op;
                        clearInputBuffer();
                        
                        HillCipher cipher(size);
                        
                        if (op == 1) {
                            std::string text = inputText("Введите текст для шифрования: ");
                            cipher.encrypt(text, key);
                        } else if (op == 2) {
                            std::string text = inputText("Введите текст для расшифрования: ");
                            cipher.decrypt(text, key);
                        }
                        
                        pressEnterToContinue();
                        break;
                    }
                    
                    case 2: {
                        std::cout << Color::BOLD << Color::MAGENTA << "\n  ╔════════════════════════════════════════╗" << Color::RESET;
                        std::cout << Color::BOLD << Color::MAGENTA << "\n  ║       РЕКУРРЕНТНЫЙ ШИФР ХИЛЛА          ║" << Color::RESET;
                        std::cout << Color::BOLD << Color::MAGENTA << "\n  ╚════════════════════════════════════════╝" << Color::RESET << std::endl;
                        
                        int size = getMatrixSize();
                        
                        std::cout << Color::CYAN << "\n  Выберите способ ввода матриц:" << Color::RESET << std::endl;
                        std::cout << "    1. Ввести вручную" << std::endl;
                        std::cout << "    2. Сгенерировать автоматически" << std::endl;
                        std::cout << "  Выбор: ";
                        
                        int inputChoice;
                        std::cin >> inputChoice;
                        clearInputBuffer();
                        
                        Matrix K0(size), K1(size);
                        if (inputChoice == 1) {
                            std::cout << Color::YELLOW << "\n  Ввод матрицы K0:" << Color::RESET << std::endl;
                            K0 = inputMatrix(size, "K0 ");
                            std::cout << Color::YELLOW << "\n  Ввод матрицы K1:" << Color::RESET << std::endl;
                            K1 = inputMatrix(size, "K1 ");
                        } else {
                            K0 = MatrixGenerator::generateInvertibleMatrix(size);
                            K1 = MatrixGenerator::generateInvertibleMatrix(size);
                            std::cout << Color::GREEN << "\n  Сгенерированная матрица K0:" << Color::RESET << std::endl;
                            K0.print();
                            std::cout << Color::GREEN << "\n  Сгенерированная матрица K1:" << Color::RESET << std::endl;
                            K1.print();
                        }
                        
                        std::cout << Color::CYAN << "\n  Выберите операцию:" << Color::RESET << std::endl;
                        std::cout << "    1. Зашифровать" << std::endl;
                        std::cout << "    2. Расшифровать" << std::endl;
                        std::cout << "  Выбор: ";
                        
                        int op;
                        std::cin >> op;
                        clearInputBuffer();
                        
                        HillCipher cipher(size);
                        
                        if (op == 1) {
                            std::string text = inputText("Введите текст для шифрования: ");
                            cipher.encryptRecurrent(text, K0, K1);
                        } else if (op == 2) {
                            std::string text = inputText("Введите текст для расшифрования: ");
                            cipher.decryptRecurrent(text, K0, K1);
                        }
                        
                        pressEnterToContinue();
                        break;
                    }
                    
                    case 3:
                        showAlphabet();
                        pressEnterToContinue();
                        break;
                        
                    case 4:
                        showHelp();
                        pressEnterToContinue();
                        break;
                        
                    default:
                        std::cout << Color::RED << "    Неверный выбор!" << Color::RESET << std::endl;
                        pressEnterToContinue();
                }
            } catch (const std::exception& e) {
                std::cout << Color::RED << "\n    ОШИБКА: " << e.what() << Color::RESET << std::endl;
                pressEnterToContinue();
            }
            
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
                printHeader();
        }
    }
};

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #else
        setlocale(LC_ALL, "en_US.UTF-8");
    #endif
    
    UserInterface ui;
    ui.run();
    
    return 0;
}
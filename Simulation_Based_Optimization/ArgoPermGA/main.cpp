#include <iostream>
#include <vector>
#include <random>
#include "ArgoPermutationGA.h"

int main(int argc, char *argv[]) {
    try {
        // 첫 번째 인수를 확인하고, 0 또는 1이 아니면 예외 발생
        if (argc != 2 || (std::stoi(argv[1]) != 0 && std::stoi(argv[1]) != 1)) {
            throw std::invalid_argument("Invalid argument. The first argument must be 0 or 1.");
        }

        int firstArgument = std::stoi(argv[1]);
        bool flag = (firstArgument == 1);

        // Argument 값 출력
        std::cout << "First Argment value: " << firstArgument << std::endl;

        // 무사히 종료된 경우 메시지 출력
        ArgoPermutationGA ga(2, flag);
        ga.exportToJson();
        std::cout << "Success." << std::endl;
        return 0;
    } catch (const std::invalid_argument &e) {
        std::cerr << "오류: " << e.what() << std::endl;
        return 1; // 비정상 종료
    } catch (const std::exception &e) {
        std::cerr << "예외가 발생했습니다: " << e.what() << std::endl;
        return 1; // 비정상 종료
    }

}

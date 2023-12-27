# 클라우드 인프라에서 Argo Workflow를 활용한 SBO 스케줄링
대한전자공학회, IEEE Daejun Section, 한국통신학회 주관 학술대회 CEIC-2023 (23.12.07 논문우수상 수상)

.## Stack
NaverCloudPlatform NKS, Argo Workflow, Python, C++

## Abstract
C++로 작성된 Genetic Alrorithm 기반 SBO를 Argo Workflow의 DAG를 통해 시뮬레이션을 진행한다.
진행된 각 Generation의 결과값을 Optimization하고 다시 DAG를 통해 시뮬레이션이 반복적으로 진행된다.
최적의 해를 찾는 중지 기준에 도달할 때까지 SBO는 반복적으로 진행된다.

DAG의 특성상 순차적인 실행 흐름을 갖는다. DAG의 비순환적인 특성은 한 번 실행되고 완료된 작업이 다시 시작되는 순환적인 프로세스를 내포하는 SBO의 반복적인 성격을 모델링하는데 한계를 지닌다.
이에 Python기반의 변환기를 설계하여 반복적인 SBO의 실행 흐름을 DAG로 표현하였다. /DAG_Converter/DAG.py 가 변환기의 코드이다.

## Node Spec
Worker Node 갯수 : 4
RAM : 128GB
CPU : Intel® Xeon® Gold 5220@2.2GHz (core 32)
SSD : 50GB

## Result


## Software version
Argo workflow : v3.5.1
Docker : 24.0.7
Cmake : 3.28.0-rc4
# 베이스 이미지 선택
FROM ubuntu:latest

# 필요한 패키지 업데이트 및 설치
RUN apt-get update && \
    apt-get install -y cmake g++ make

# 작업 디렉토리 설정
WORKDIR /app

# 소스 코드 복사
COPY . /app

# CMake 및 make를 사용하여 프로젝트 빌드
RUN cmake . && \
    make

# 스크립트 파일 복사 (entrypoint.sh 또는 유사한 파일이어야 합니다)
COPY entrypoint.sh /app

# 스크립트 파일 실행 권한 부여
RUN chmod +x /app/entrypoint.sh

# 컨테이너 시작 시 실행할 명령어 설정
CMD ["/app/entrypoint.sh"]

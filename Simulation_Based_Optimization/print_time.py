from datetime import datetime
import os

# 현재 시간을 저장
start_time = datetime.now()

os.system('docker run -d otober99/argo_workflow_ga:12.0 bash -c "/app/bin/ArgoOptExec 0&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"
+ "/app/bin/ArgoOptExec 1&&/app/bin/SimRunner 0 && /app/bin/SimRunner 1 && /app/bin/SimRunner 2 &&/app/bin/SimRunner 3 &&/app/bin/SimRunner 4 &&/app/bin/SimRunner 5 &&/app/bin/SimRunner 6 &&/app/bin/SimRunner 7 &&/app/bin/SimRunner 8 &&/app/bin/SimRunner 9 &&/app/bin/SimRunner 10 &&/app/bin/SimRunner 1 1&&/app/bin/ArgoOptExec 1"')


end_time = datetime.now()
time_difference = end_time - start_time

print("Time difference: ", time_difference)
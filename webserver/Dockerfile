# syntax=docker/dockerfile:1
FROM python:3.10.10-alpine3.17

WORKDIR /stargazer
COPY . /stargazer
RUN pip3 install -r requirements.txt

CMD ["python3", "-m", "gunicorn", "server:app","-k", "uvicorn.workers.UvicornWorker", "--bind", "0.0.0.0:8000"]


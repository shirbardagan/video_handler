FROM python:3.12

ENV SYSTEM_MAX_USERS=3
ENV SYSTEM_USE_GPU=True

WORKDIR /opt
COPY package.json .

WORKDIR /app
COPY . .

RUN pip install -r requirements.txt

EXPOSE 8080

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8080"]


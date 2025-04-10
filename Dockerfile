FROM python:3.12

WORKDIR /opt
COPY package.json .

WORKDIR /app
COPY . .

RUN pip install -r requirements.txt

RUN chmod +x /start.sh

EXPOSE 8080

CMD ["/start_app.sh"]

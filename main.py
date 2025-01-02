from app_instance import app, start_server
from routes import command, websocket, index


app.include_router(command.router)
app.include_router(websocket.router)
app.include_router(index.router)

if __name__ == "__main__":
    start_server()
from app_instance import app, start_server
from routes import command, websocket, index, setup, liveness_readiness

app.include_router(command.router)
app.include_router(websocket.router)
app.include_router(index.router)
app.include_router(setup.router)
app.include_router(liveness_readiness.router)

if __name__ == "__main__":
    start_server()

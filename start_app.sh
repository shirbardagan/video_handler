#!/bin/bash

PORT=${VH_PORT:-8080}

exec uvicorn main:app --host '0.0.0.0' --port "$PORT"

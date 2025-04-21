#!/bin/bash

PORT=${SYSTEM_PORT:-5050}

exec uvicorn main:app --host '0.0.0.0' --port "$PORT"

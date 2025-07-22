FROM mcr.microsoft.com/devcontainers/base:ubuntu

RUN apt-get update && apt-get install -y curl docker.io docker-compose build-essential \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN curl --fail -L https://app.mayhem.security/cli/Linux/install.sh | sh && mayhem --version

USER vscode
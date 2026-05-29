FROM ubuntu:22.04

ARG PYRE_CHECK_REF=888fabee67b22bc2173f3ab08e88920948a91493

ENV DEBIAN_FRONTEND=noninteractive \
    PYTHONUNBUFFERED=1 \
    PIP_NO_CACHE_DIR=1 \
    PYRE_HOME=/home/pyre/pyre-check \
    PYRE_BINARY=/home/pyre/pyre-check/source/_build/default/main.exe \
    PYRE_TYPESHED=/home/pyre/pyre-check/stubs/typeshed/typeshed \
    PYTHONPATH=/home/pyre

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        curl \
        git \
        libgmp-dev \
        m4 \
        opam \
        pkg-config \
        python3 \
        python3-pip \
        python3-venv \
        rsync \
        unzip \
        vim \
    && rm -rf /var/lib/apt/lists/*

RUN useradd --create-home --shell /bin/bash pyre

USER pyre
WORKDIR /home/pyre

RUN git clone https://github.com/facebook/pyre-check.git "${PYRE_HOME}" \
    && cd "${PYRE_HOME}" \
    && git checkout --detach "${PYRE_CHECK_REF}"

WORKDIR ${PYRE_HOME}

RUN python3 -m pip install --user --upgrade pip \
    && python3 -m pip install --user -r requirements.txt django-stubs==6.0.5 fb-sapp==0.5.9

RUN ./scripts/setup.sh --local --no-tests

USER root

RUN printf '%s\n' \
    '#!/usr/bin/env bash' \
    'set -euo pipefail' \
    'export PYRE_BINARY="${PYRE_BINARY:-/home/pyre/pyre-check/source/_build/default/main.exe}"' \
    'export PYRE_TYPESHED="${PYRE_TYPESHED:-/home/pyre/pyre-check/stubs/typeshed/typeshed}"' \
    'export PYTHONPATH="/home/pyre/pyre-check:${PYTHONPATH:-}"' \
    'exec python3 -m client.pyre "$@"' \
    > /usr/local/bin/pyre \
    && chmod +x /usr/local/bin/pyre

ENV PATH=/home/pyre/.local/bin:${PATH}

USER pyre
WORKDIR /workspace/kalmarnotes

CMD ["bash"]

FROM python:3.9-slim

WORKDIR /app

RUN apt-get update && apt-get install -y --no-install-recommends \
    libnss3 \
    libfontconfig1 \
    chromium \
    chromium-driver \
    varnish \
    supervisor \
 && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY src/ .
COPY default.vcl /
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

ENV DISPLAY=:99
ENV CHROME_OPTIONS="--headless --disable-gpu --no-sandbox --disable-dev-shm-usage"
ENV HOSTNAME=localhost

# challenge secrets moved to docker-compose
EXPOSE 80

CMD ["/usr/bin/supervisord"]

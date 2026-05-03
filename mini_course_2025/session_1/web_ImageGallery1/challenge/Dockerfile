FROM node:22-bookworm-slim

RUN apt-get update && apt-get install -y \
    ca-certificates fonts-liberation libappindicator3-1 libasound2 libatk-bridge2.0-0 \
    libatk1.0-0 libc6 libcairo2 libcups2 libdbus-1-3 libexpat1 libfontconfig1 \
    libgbm1 libgcc1 libglib2.0-0 libgtk-3-0 libnspr4 libnss3 libpango-1.0-0 \
    libpangocairo-1.0-0 libstdc++6 libx11-6 libx11-xcb1 libxcb1 libxcomposite1 \
    libxcursor1 libxdamage1 libxext6 libxfixes3 libxi6 libxrandr2 libxrender1 \
    libxss1 libxtst6 lsb-release wget xdg-utils --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*

ENV NODE_ENV=production
# Keep Puppeteer's Chromium download inside /app so it's accessible after USER switch
ENV PUPPETEER_CACHE_DIR=/app/.cache

WORKDIR /app
RUN mkdir /app/public

COPY src/package*.json .
RUN npm install --omit=dev

COPY src/ .

RUN useradd -ms /bin/bash user && \
    chown -R user:user /app && \
    chmod -R +rx /app
USER user

CMD ["node", "app.js"]

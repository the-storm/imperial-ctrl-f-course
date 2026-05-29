FROM semgrep/semgrep:latest

WORKDIR /scan

ENV SEMGREP_SEND_METRICS=off

# The host levels directory is mounted here at runtime.
VOLUME ["/levels", "/scan/reports"]

ENTRYPOINT ["semgrep"]
CMD ["scan", "--config", "p/default", "--config", "p/security-audit", "--config", "p/secrets", "--error", "--json", "--output", "/scan/reports/semgrep-results.json", "/levels"]

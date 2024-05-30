# 4u-cdn
A simple user-content CDN for 4u services

## Requirements
- Docker
- C++17 or higher
- AWS S3 Bucket
- 4gb of memory (or more)
- An SSL certificate and key

## Build
```bash
sudo docker build --tag "4u-cdn" . --memory="2g" --memory-swap="4g"
```
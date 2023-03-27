# syntax=docker/dockerfile:1
FROM nginx:1.23.3-alpine

WORKDIR /stargazer
COPY . /stargazer
RUN apk update
RUN apk add nodejs npm
RUN npm i
RUN npm run build
RUN cp /stargazer/default.conf /etc/nginx/conf.d/default.conf


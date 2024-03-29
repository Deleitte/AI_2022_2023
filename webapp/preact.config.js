/**
 * Function that mutates the original webpack config.
 * Supports asynchronous changes when a promise is returned (or it's an async function).
 *
 * @param {import('preact-cli').Config} config - original webpack config
 * @param {import('preact-cli').Env} env - current environment and options pass to the CLI
 * @param {import('preact-cli').Helpers} helpers - object with useful helpers for working with the webpack config
 * @param {Record<string, unknown>} options - this is mainly relevant for plugins (will always be empty in the config), default to an empty object
 */
export default (config, env, helpers, options) => {
  if (config.devServer) {
    config.devServer.proxy = [
        {
            // proxy requests matching a pattern:
            path: '/api/**',
      
            // where to proxy to:
            target: process.env.API_URL || 'http://127.0.0.1:8000',
      
            // optionally change Origin: and Host: headers to match target:
            changeOrigin: true,
            changeHost: true,
            secure: false,
      
            pathRewrite(path, request) {
              // common: remove first path segment: (/api/**)
              // eslint-disable-next-line prefer-template, no-useless-escape
              return "/" + path.replace(/^\/[^\/]+\//, "");
            },
        }
    ];
  }
};
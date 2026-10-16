function apiEndpoint(base, route) {
  const prefix = String(base).replace(/\/+$/, '');
  const suffix = String(route).replace(/^\/+/, '');
  return `${prefix}/${suffix}`;
}

module.exports = { apiEndpoint };

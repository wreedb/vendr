project('mbedTLS', 'c',
  meson_version: '>= 1.3.0',
  version: '3.6.4',
  default_options: [
    'c_std=c99',
    'warning_level=0',
    'default_library=static',
    'strip=true',
    'buildtype=plain'])


tls_sources = files(
  'library/debug.c',
  'library/mps_reader.c',
  'library/mps_trace.c',
  'library/net_sockets.c',
  'library/ssl_cache.c',
  'library/ssl_ciphersuites.c',
  'library/ssl_client.c',
  'library/ssl_cookie.c',
  'library/ssl_debug_helpers_generated.c',
  'library/ssl_msg.c',
  'library/ssl_ticket.c',
  'library/ssl_tls.c',
  'library/ssl_tls12_client.c',
  'library/ssl_tls12_server.c',
  'library/ssl_tls13_keys.c',
  'library/ssl_tls13_client.c',
  'library/ssl_tls13_server.c',
  'library/ssl_tls13_generic.c')

crypto_sources = files(
  'library/aes.c',
  'library/aesni.c',
  'library/aesce.c',
  'library/aria.c',
  'library/asn1parse.c',
  'library/asn1write.c',
  'library/base64.c',
  'library/bignum.c',
  'library/bignum_core.c',
  'library/bignum_mod.c',
  'library/bignum_mod_raw.c',
  'library/block_cipher.c',
  'library/camellia.c',
  'library/ccm.c',
  'library/chacha20.c',
  'library/chachapoly.c',
  'library/cipher.c',
  'library/cipher_wrap.c',
  'library/cmac.c',
  'library/constant_time.c',
  'library/ctr_drbg.c',
  'library/des.c',
  'library/dhm.c',
  'library/ecdh.c',
  'library/ecdsa.c',
  'library/ecjpake.c',
  'library/ecp.c',
  'library/ecp_curves.c',
  'library/ecp_curves_new.c',
  'library/entropy.c',
  'library/entropy_poll.c',
  'library/error.c',
  'library/gcm.c',
  'library/hkdf.c',
  'library/hmac_drbg.c',
  'library/lmots.c',
  'library/lms.c',
  'library/md.c',
  'library/md5.c',
  'library/memory_buffer_alloc.c',
  'library/nist_kw.c',
  'library/oid.c',
  'library/padlock.c',
  'library/pem.c',
  'library/pk.c',
  'library/pk_ecc.c',
  'library/pk_wrap.c',
  'library/pkcs12.c',
  'library/pkcs5.c',
  'library/pkparse.c',
  'library/pkwrite.c',
  'library/platform.c',
  'library/platform_util.c',
  'library/poly1305.c',
  'library/psa_crypto.c',
  'library/psa_crypto_aead.c',
  'library/psa_crypto_cipher.c',
  'library/psa_crypto_client.c',
  'library/psa_crypto_driver_wrappers_no_static.c',
  'library/psa_crypto_ecp.c',
  'library/psa_crypto_ffdh.c',
  'library/psa_crypto_hash.c',
  'library/psa_crypto_mac.c',
  'library/psa_crypto_pake.c',
  'library/psa_crypto_rsa.c',
  'library/psa_crypto_se.c',
  'library/psa_crypto_slot_management.c',
  'library/psa_crypto_storage.c',
  'library/psa_its_file.c',
  'library/psa_util.c',
  'library/ripemd160.c',
  'library/rsa.c',
  'library/rsa_alt_helpers.c',
  'library/sha1.c',
  'library/sha256.c',
  'library/sha512.c',
  'library/sha3.c',
  'library/threading.c',
  'library/timing.c',
  'library/version.c',
  'library/version_features.c',
  '3rdparty/everest/library/everest.c',
  '3rdparty/everest/library/x25519.c',
  '3rdparty/everest/library/Hacl_Curve25519_joined.c',
  '3rdparty/p256-m/p256-m_driver_entrypoints.c',
  '3rdparty/p256-m/p256-m/p256-m.c')

x509_sources = files(
  'library/x509.c',
  'library/x509_create.c',
  'library/x509_crl.c',
  'library/x509_crt.c',
  'library/x509_csr.c',
  'library/x509write.c',
  'library/x509write_crt.c',
  'library/x509write_csr.c',
  'library/pkcs7.c')

x509_lib = library(
  'mbedx509',
  x509_sources,
  include_directories: ['library', 'include'],
  install: true)

crypto_lib = library(
  'mbedcrypto',
  crypto_sources,
  include_directories: ['library', 'include'],
  install: true)

tls_lib = library(
  'mbedtls',
  tls_sources,
  include_directories: ['library', 'include'],
  install: true)

install_subdir('include/mbedtls', install_dir: get_option('includedir'))
install_subdir('include/psa', install_dir: get_option('includedir'))


mbedtls_dep = declare_dependency(
  link_with: tls_lib,
  include_directories: ['library', 'include'])

mbedcrypto_dep = declare_dependency(
  link_with: crypto_lib,
  include_directories: ['library', 'include'])

mbedx509_dep = declare_dependency(
  link_with: x509_lib,
  include_directories: ['library', 'include'])

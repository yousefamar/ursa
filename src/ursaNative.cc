// Copyright 2012 The Obvious Corporation.

#include "ursaNative.h"
#include <node_buffer.h>

// FIXME: Do we need all of these?
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/hmac.h>
#include <openssl/engine.h>
#include <openssl/rand.h>

using namespace v8;


/*
 * Initialization and binding
 */

/**
 * Top-level initialization function.
 */
void init(Handle<Object> target) {
    RsaWrap::InitClass(target);
}

NODE_MODULE(ursaNative, init)

/**
 * Helper for prototype binding.
 */
#define BIND(proto, highName, lowName) \
    (proto)->Set(String::NewSymbol(#highName), \
        FunctionTemplate::New(lowName)->GetFunction())

/**
 * Initialize the bindings for this class.
 */
void RsaWrap::InitClass(Handle<Object> target) {
    Local<String> className = String::NewSymbol("RsaWrap");

    // Basic instance setup
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

    tpl->SetClassName(className);
    tpl->InstanceTemplate()->SetInternalFieldCount(1); // required by ObjectWrap

    // Prototype method bindings
    Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

    BIND(proto, generatePrivateKey, GeneratePrivateKey);
    BIND(proto, getExponent,        GetExponent);
    BIND(proto, getModulus,         GetModulus);
    BIND(proto, getPrivateKeyPem,   GetPrivateKeyPem);
    BIND(proto, getPublicKeyPem,    GetPublicKeyPem);
    BIND(proto, privateDecrypt,     PrivateDecrypt);
    BIND(proto, privateEncrypt,     PrivateEncrypt);
    BIND(proto, publicDecrypt,      PublicDecrypt);
    BIND(proto, publicEncrypt,      PublicEncrypt);
    BIND(proto, setPrivateKeyPem,   SetPrivateKeyPem);
    BIND(proto, setPublicKeyPem,    SetPublicKeyPem);

    // Store the constructor in the target bindings.
    target->Set(className, Persistent<Function>::New(tpl->GetFunction()));
}


/*
 * Helper functions
 */

/**
 * Schedule the current SSL error as a higher-level exception.
 */
static void scheduleSslException() {
    char *err = ERR_error_string(ERR_get_error(), NULL);
    Local<Value> exception = Exception::Error(String::New(err));
    ThrowException(exception);
}

/*
 * RsaWrap implementation
 */

/**
 * Straightforward constructor. Nothing much to initialize, other than
 * to ensure that our one instance variable is sanely NULLed.
 */
RsaWrap::RsaWrap() {
    rsa = NULL;
}

/**
 * Destructor, which is called automatically via the ObjectWrap mechanism
 * when the corresponding high-level object gets gc'ed.
 */
RsaWrap::~RsaWrap() {
    if (rsa != NULL) {
	RSA_free(rsa);
    }
}

/**
 * Get a Buffer out of the zeroth argument. Returns a non-null pointer
 * on success. On failure, schedules an exception and returns NULL.
 */
BIO *RsaWrap::getArg0Buffer(const Arguments& args) {
    if (args.Length() < 1) {
	ThrowException(Exception::TypeError(String::New(
            "Missing Buffer argument.")));
	return NULL;
    }

    if (!node::Buffer::HasInstance(args[0])) {
	ThrowException(Exception::TypeError(String::New(
            "First argument is not a Buffer.")));
	return NULL;
    }

    Local<Object> buf = args[0]->ToObject();
    char *data = node::Buffer::Data(buf);
    ssize_t length = node::Buffer::Length(buf);

    BIO *bp = BIO_new_mem_buf(data, length);

    if (bp == NULL) {
	scheduleSslException();
    }

    return bp;
}

/**
 * Get an (RsaWrap *) out of the given arguments, expecting the
 * underlying (RSA *) to be non-null and more specifically a private
 * key. Returns a non-null pointer on success. On failure, schedules
 * an exception and returns null.
 */
RsaWrap *RsaWrap::unwrapExpectPrivateKey(const Arguments& args) {
    RsaWrap *obj = unwrapExpectSet(args);

    if ((obj != NULL) && (obj->rsa->d != NULL)) {
	return obj;
    }

    Local<Value> exception =
	Exception::Error(String::New("Rsa not a private key."));
    ThrowException(exception);
    return NULL;
}

/**
 * Get an (RsaWrap *) out of the given arguments, expecting the underlying
 * (RSA *) to be non-null. Returns a non-null pointer on success. On failure,
 * schedules an exception and returns null.
 */
RsaWrap *RsaWrap::unwrapExpectSet(const Arguments& args) {
    RsaWrap *obj = ObjectWrap::Unwrap<RsaWrap>(args.Holder());

    if (obj->rsa != NULL) {
	return obj;
    }

    Local<Value> exception = Exception::Error(String::New("Rsa not yet set."));
    ThrowException(exception);
    return NULL;
}

/**
 * Get an (RsaWrap *) out of the given arguments, expecting the underlying
 * (RSA *) to be null. Returns a non-null pointer on success. On failure,
 * schedules an exception and returns null.
 */
RsaWrap *RsaWrap::unwrapExpectUnset(const Arguments& args) {
    RsaWrap *obj = ObjectWrap::Unwrap<RsaWrap>(args.Holder());

    if (obj->rsa == NULL) {
	return obj;
    }

    Local<Value> exception = Exception::Error(String::New("Rsa already set."));
    ThrowException(exception);
    return NULL;
}

/**
 * Construct an empty instance.
 */
Handle<Value> RsaWrap::New(const Arguments& args) {
    RsaWrap *obj = new RsaWrap();
    obj->Wrap(args.This());

    return args.This();
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::GeneratePrivateKey(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectUnset(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::GetExponent(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectSet(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::GetModulus(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectSet(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::GetPrivateKeyPem(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectPrivateKey(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::GetPublicKeyPem(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectSet(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::PrivateDecrypt(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectPrivateKey(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::PrivateEncrypt(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectPrivateKey(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::PublicDecrypt(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectSet(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::PublicEncrypt(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectSet(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

// FIXME: Need documentation.
Handle<Value> RsaWrap::SetPrivateKeyPem(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectUnset(args);
    if (obj == NULL) { return Undefined(); }

    // FIXME: Need real implementation.
    return scope.Close(String::New("world"));
}

/**
 * Sets the underlying RSA object to correspond to the given
 * public key (a Buffer of PEM format data). This throws an
 * exception if the underlying RSA had previously been set.
 */
Handle<Value> RsaWrap::SetPublicKeyPem(const Arguments& args) {
    HandleScope scope;

    RsaWrap *obj = unwrapExpectUnset(args);
    if (obj == NULL) { return Undefined(); }

    BIO *bp = getArg0Buffer(args);
    if (bp == NULL) { return Undefined(); }

    obj->rsa = PEM_read_bio_RSA_PUBKEY(bp, NULL, NULL, NULL);

    if (obj->rsa == NULL) {
	scheduleSslException();
    }

    BIO_free(bp);
    return Undefined();
}

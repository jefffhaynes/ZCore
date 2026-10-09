#pragma once

#include <zephyr/net/socket.h>
#include <zephyr/net/tls_credentials.h>
#include "Array.h"
#include "CoreString.h"
#include "ErrorConverter.h"
#include "ReturnCode.h"
#include "Span.h"
#include "SpanWriter.h"

#if !defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
#error "Tls needs CONFIG_NET_SOCKETS_SOCKOPT_TLS"
#endif

// The certificates and keys the stack holds for TLS, each kept under a tag that connections name.
// They're DER, and the stack keeps the pointer, so they must outlive every connection.
class TlsCredentials
{
public:
    // A certificate authority that connections under `tag` trust.
    static ReturnCode AddAuthority(int tag, Span<const uint8_t> certificate)
    {
        return Add(tag, TLS_CREDENTIAL_CA_CERTIFICATE, certificate);
    }

    // What a server under `tag` presents: its certificate, and the key that goes with it.
    static ReturnCode AddCertificate(int tag, Span<const uint8_t> certificate)
    {
        return Add(tag, TLS_CREDENTIAL_PUBLIC_CERTIFICATE, certificate);
    }

    static ReturnCode AddPrivateKey(int tag, Span<const uint8_t> key)
    {
        return Add(tag, TLS_CREDENTIAL_PRIVATE_KEY, key);
    }

private:
    static ReturnCode Add(int tag, enum tls_credential_type type, Span<const uint8_t> credential)
    {
        auto err = tls_credential_add(tag, type, credential.GetData(), credential.GetLength());
        return ErrorConverter::Convert(err);
    }
};

// How a connection is secured: the credentials it trusts, and the name its server must prove.
struct TlsOptions
{
    static constexpr int None = -1;

    // The longest a DNS name gets.
    static constexpr uint32_t MaxHostNameLength = 253;

    // The tag of the certificate authorities to trust (see TlsCredentials), or None for no TLS.
    int Tag = None;

    // Checked against the server's certificate, and sent so a shared address serves the right one.
    // Empty leaves the name unchecked.
    String HostName;

    constexpr TlsOptions()
    {
    }

    constexpr TlsOptions(int tag, String hostName) : Tag(tag), HostName(hostName)
    {
    }

    constexpr bool IsEnabled() const
    {
        return Tag != None;
    }

    // Applies to a TLS socket that hasn't connected yet.
    ReturnCode Apply(int descriptor) const
    {
        sec_tag_t tag = Tag;

        if(zsock_setsockopt(descriptor, SOL_TLS, TLS_SEC_TAG_LIST, &tag, sizeof(tag)) < 0)
        {
            return ErrorConverter::Convert(-errno);
        }

        if(HostName.IsEmpty())
        {
            return ReturnCode::Success;
        }

        // The stack reads the name as a C string.
        Array<char, MaxHostNameLength + 1> name;
        SpanWriter<char> writer(name);

        auto rc = writer.Write(HostName);
        CHECK_RETURN_CODE(rc);

        rc = writer.Write('\0');
        CHECK_RETURN_CODE(rc);

        if(zsock_setsockopt(descriptor, SOL_TLS, TLS_HOSTNAME, name.GetData(), HostName.GetLength()) < 0)
        {
            return ErrorConverter::Convert(-errno);
        }

        return ReturnCode::Success;
    }
};

#include <fc/crypto/bigint.hpp>
#include <fc/io/sstream.hpp>
#include <fc/log/logger.hpp>

namespace fc
{
    fc::string to_base36( const char* data, size_t len )
    {
       if( len == 0 ) return fc::string();
       fc::bigint value( data, len );
       auto base36 = "0123456789abcdefghijklmnopqrstuvwxyz";
       std::vector<char> out( static_cast<size_t>(len * 1.6) + 1 );
       int pos = out.size() - 1;
       out[pos] = '\0';
       fc::bigint _36(36);
       do {
         if( value ) {
           --pos;
           out[pos] = base36[(value % _36).to_int64()];
         }
       } while (value /= _36);
       while (*data++ == 0) {
           out[--pos] = '0';
       }
       return &out[pos]; //fc::string( &out[pos], out.size() - pos);
    }

    fc::string to_base36( const std::vector<char>& vec )
    {
      return to_base36( (const char*)vec.data(), vec.size() );
    }

    std::vector<char> from_base36( const fc::string& b36 )
    {
        if ( b36.empty() ) {
           std::vector<char> empty;
           return empty;
        }

       fc::bigint value;

       fc::bigint pos = 0; 
       fc::bigint _36(36);
       for( auto itr = b36.rbegin(); itr != b36.rend(); ++itr )
       {
          if( *itr >= '0' && *itr <= '9' )
              value = value +  _36.exp(pos) * fc::bigint(*itr - '0');
          else if( *itr >= 'a' && *itr <= 'z' )
              value = value + (_36.exp(pos) *  fc::bigint(10+*itr - 'a'));
          else if( *itr >= 'A' && *itr <= 'Z' )
              value = value + (_36.exp(pos) *  fc::bigint(10+*itr - 'A'));
          else
          {
             wlog("unknown '${char}'", ("char",fc::string(&*itr,1)) );
          }
          ++pos;
       }

       std::vector<char> bytes = value;
       int leading_zeros = 0, len = bytes.size();
       const char *in = b36.c_str();
       while (*in++ == '0') { leading_zeros++; }
       char* first = bytes.data();
       while (*first == 0) { first++; len--; }
       std::vector<char> result;
       result.resize(leading_zeros + len, 0);
       memcpy( result.data() + leading_zeros, first, len );
       return result;
    }
}

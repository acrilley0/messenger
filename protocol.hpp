#include <string>
#include <boost/archive/archive_exception.hpp>
#include <boost/asio.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp> // This has to be included because the data we are trying to serialize includes strings

typedef struct payload_t {
  std::string user;
  std::string msg_content;

  friend class boost::serialization::access;
  template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
      ar & user;
      ar & msg_content;
    }
} payload_t;

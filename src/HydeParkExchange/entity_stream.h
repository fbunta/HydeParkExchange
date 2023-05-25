#include <iostream>
#include "order.h"
#include "abstract_order_factory.h"

using hpx::order;
using hpx::ioc_order;
using hpx::market_order;
using hpx::limit_order;
using hpx::abstract_factory;
using hpx::concrete_factory;
using hpx::order_side;
using hpx::trading_entity;

namespace hpx {
    template<class CharT, class Traits = std::char_traits<CharT> >
    class entity_stream : public std::basic_ostream<CharT, Traits>
    {
    public:
        static const int entity_index;

        entity_stream(std::basic_ostream<CharT, Traits>& ostr, const trading_entity entity) :
            std::basic_ostream<CharT, Traits>(ostr.rdbuf())
        {
            trading_entity* e = new trading_entity(entity);
            this->pword(entity_index) = e;
        }

        void myfn(order* o)
        {
            trading_entity a = *static_cast<trading_entity*>(this->pword(entity_index));
            if (a == o->trading_entity_) {
                *this << *o;
            }
        }
    };

    // each specialization of mystream obtains a unique index from xalloc()
    template<class CharT, class Traits>
    const int entity_stream<CharT, Traits>::entity_index = std::ios_base::xalloc();

    // This I/O manipulator will be able to recognize ostreams that are entity_streams
    // by looking up the pointer stored in pword
    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>&
    mymanip(std::basic_ostream<CharT, Traits>& os, order* order_)
    {
        static_cast<entity_stream<CharT, Traits>&>(os).myfn(order_);
        return os;
    }
}
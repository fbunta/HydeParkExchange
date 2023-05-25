#ifndef ENTITY_STREAM_H
#   define ENTITY_STREAM_H
#include <iostream>
#include "order.h"
#include "fill.h"

using hpx::order;
using hpx::fill;

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

        void myfn(fill* f)
        {
            trading_entity a = *static_cast<trading_entity*>(this->pword(entity_index));
            if (a == f->buy.entity_) {
                *this << f->buy;
            }
            else if (a == f->sell.entity_) {
                *this << f->sell;
            }
        }
    };

    // each specialization of mystream obtains a unique index from xalloc()
    template<class CharT, class Traits>
    const int entity_stream<CharT, Traits>::entity_index = std::ios_base::xalloc();


    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>&
    entity_filtered_manip(std::basic_ostream<CharT, Traits>& os, order* order_)
    {
        static_cast<entity_stream<CharT, Traits>&>(os).myfn(order_);
        return os;
    }

    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>&
        entity_filtered_manip(std::basic_ostream<CharT, Traits>& os, fill* fill_)
    {
        static_cast<entity_stream<CharT, Traits>&>(os).myfn(fill_);
        return os;
    }
}
#endif // !ENTITY_STREAM_H

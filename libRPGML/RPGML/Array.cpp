/* This file is part of RPGML.
 *
 * Copyright (c) 2015, Gunnar Payer, All rights reserved.
 *
 * RPGML is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#include "Array.h"

#include "Frame.h"
#include "Function.h"
#include "Node.h"
#include "Sequence.h"

namespace RPGML {

template class Array< bool                       >;
template class Array< uint8_t                    >;
template class Array< int8_t                     >;
template class Array< uint16_t                   >;
template class Array< int16_t                    >;
template class Array< uint32_t                   >;
template class Array< int32_t                    >;
template class Array< uint64_t                   >;
template class Array< int64_t                    >;
template class Array< float                      >;
template class Array< double                     >;
template class Array< String                     >;
template class Array< CountPtr< Frame          > >;
template class Array< CountPtr< Function       > >;
template class Array< CountPtr< Node           > >;
template class Array< CountPtr< Output         > >;
template class Array< CountPtr< Input          > >;
template class Array< CountPtr< Param          > >;
template class Array< CountPtr< const Sequence > >;
template class Array< CountPtr< ArrayBase      > >;

} // namespace RPGML

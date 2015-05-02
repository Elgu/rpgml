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
#include "ArrayData.h"

#include "Frame.h"
#include "Function.h"
#include "Node.h"
#include "Sequence.h"
#include "ArrayBase.h"

namespace RPGML {

template class ArrayData< bool                       >;
template class ArrayData< uint8_t                    >;
template class ArrayData< int8_t                     >;
template class ArrayData< uint16_t                   >;
template class ArrayData< int16_t                    >;
template class ArrayData< uint32_t                   >;
template class ArrayData< int32_t                    >;
template class ArrayData< uint64_t                   >;
template class ArrayData< int64_t                    >;
template class ArrayData< float                      >;
template class ArrayData< double                     >;
template class ArrayData< String                     >;
template class ArrayData< CountPtr< Frame          > >;
template class ArrayData< CountPtr< Function       > >;
template class ArrayData< CountPtr< Node           > >;
template class ArrayData< CountPtr< Output         > >;
template class ArrayData< CountPtr< Input          > >;
template class ArrayData< CountPtr< Param          > >;
template class ArrayData< CountPtr< const Sequence > >;
template class ArrayData< CountPtr< ArrayBase      > >;

template class ArrayContainer< bool                       >;
template class ArrayContainer< uint8_t                    >;
template class ArrayContainer< int8_t                     >;
template class ArrayContainer< uint16_t                   >;
template class ArrayContainer< int16_t                    >;
template class ArrayContainer< uint32_t                   >;
template class ArrayContainer< int32_t                    >;
template class ArrayContainer< uint64_t                   >;
template class ArrayContainer< int64_t                    >;
template class ArrayContainer< float                      >;
template class ArrayContainer< double                     >;
template class ArrayContainer< String                     >;
template class ArrayContainer< CountPtr< Frame          > >;
template class ArrayContainer< CountPtr< Function       > >;
template class ArrayContainer< CountPtr< Node           > >;
template class ArrayContainer< CountPtr< Output         > >;
template class ArrayContainer< CountPtr< Input          > >;
template class ArrayContainer< CountPtr< Param          > >;
template class ArrayContainer< CountPtr< const Sequence > >;
template class ArrayContainer< CountPtr< ArrayBase      > >;

template class ContainerArrayData< bool                       >;
template class ContainerArrayData< uint8_t                    >;
template class ContainerArrayData< int8_t                     >;
template class ContainerArrayData< uint16_t                   >;
template class ContainerArrayData< int16_t                    >;
template class ContainerArrayData< uint32_t                   >;
template class ContainerArrayData< int32_t                    >;
template class ContainerArrayData< uint64_t                   >;
template class ContainerArrayData< int64_t                    >;
template class ContainerArrayData< float                      >;
template class ContainerArrayData< double                     >;
template class ContainerArrayData< String                     >;
template class ContainerArrayData< CountPtr< Frame          > >;
template class ContainerArrayData< CountPtr< Function       > >;
template class ContainerArrayData< CountPtr< Node           > >;
template class ContainerArrayData< CountPtr< Output         > >;
template class ContainerArrayData< CountPtr< Input          > >;
template class ContainerArrayData< CountPtr< Param          > >;
template class ContainerArrayData< CountPtr< const Sequence > >;
template class ContainerArrayData< CountPtr< ArrayBase      > >;

} // namespace RPGML

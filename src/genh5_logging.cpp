/* GenH5
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 *
 *  Created on: 31.1.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "genh5_logging.h"

#include <iostream>

static std::function<GenH5::log::OutputSink> defaultSink =
        [](std::string const& msg){
    std::cerr << msg << '\n';
};

static std::function<GenH5::log::OutputSink> sink = defaultSink;

void
GenH5::log::clearErrorOutputSink()
{
    sink = defaultSink;
}

void
GenH5::log::registerErrorOutputSink(std::function<OutputSink> func)
{
    if (func)
    {
        sink = std::move(func);
    }
}

GenH5::log::ErrStream::~ErrStream()
{
    sink(m_stream.str());
}

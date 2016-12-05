/*
 * Copyright (C) 2016 Naver Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MenuWin.h"

#include <android/view/ViewHostWindow.h>
#include <android/view/ViewPrivate.h>

#include <platforms/StringConversion.h>

namespace android {
namespace view {

std::unique_ptr<Menu> Menu::create(View& anchor)
{
    return std::unique_ptr<Menu>(new MenuWin(anchor));
}

MenuWin::MenuWin(View& anchor)
    : Menu(anchor)
    , m_handle(NULL)
{
    m_handle = ::CreatePopupMenu();
}

MenuWin::~MenuWin()
{
    dismiss();
}

void MenuWin::addItem(const std::shared_ptr<MenuItem>& item)
{
    UINT flags = MF_STRING;
    item->enabled() ? flags |= MF_ENABLED : flags |= MF_GRAYED;
    item->checked() ? flags |= MF_CHECKED : flags |= MF_UNCHECKED;
    ::AppendMenu(m_handle, flags, item->tag(), item->title().c_str());
}

void MenuWin::addItems(const std::vector<std::shared_ptr<MenuItem>>& items)
{
    for (const std::shared_ptr<MenuItem>& item : items)
        addItem(item);
}

void MenuWin::addSeparator()
{
    ::AppendMenu(m_handle, MF_SEPARATOR, 0, nullptr);
}

std::unique_ptr<Menu> MenuWin::addSubmenu(const std::shared_ptr<MenuItem>& item)
{
    std::unique_ptr<MenuWin> submenu(new MenuWin(m_anchor));
    ::AppendMenu(m_handle, MF_POPUP, reinterpret_cast<UINT_PTR>(submenu->handle()), item->title().c_str());
    return std::move(submenu);
}

void MenuWin::show(Point& location)
{
    HWND parentHWND = reinterpret_cast<HWND>(view::getPrivate(m_anchor).hostWindow()->windowHandle());

    POINT point;
    point.x = location.x;
    point.y = location.y;
    ::ClientToScreen(parentHWND, &point);

    int32_t selection = ::TrackPopupMenu(m_handle, TPM_RIGHTBUTTON | TPM_RETURNCMD,
        point.x, point.y, 0, parentHWND, NULL);

    if (!selection)
        return;

    m_menuItemClickListener(selection);
}

void MenuWin::dismiss()
{
    if (m_handle) {
        ::DestroyMenu(m_handle);
        m_handle = NULL;
    }
}

} // namespace view
} // namespace android

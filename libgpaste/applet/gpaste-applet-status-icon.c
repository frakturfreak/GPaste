/*
 *      This file is part of GPaste.
 *
 *      Copyright 2014 Marc-Antoine Perennou <Marc-Antoine@Perennou.com>
 *
 *      GPaste is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      GPaste is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with GPaste.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gpaste-applet-status-icon-private.h"

struct _GPasteAppletStatusIconPrivate
{
    GtkStatusIcon *icon;
    GtkMenu       *menu;

    gulong         press_id;
};

G_DEFINE_TYPE_WITH_PRIVATE (GPasteAppletStatusIcon, g_paste_applet_status_icon, G_PASTE_TYPE_APPLET_ICON)

static gboolean
g_paste_applet_status_icon_popup (GtkStatusIcon *status_icon,
                                  GdkEvent      *event,
                                  gpointer       user_data)
{
    GPasteAppletStatusIconPrivate *priv = user_data;
    GtkWidget *widget = GTK_WIDGET (priv->menu);

    gtk_widget_set_visible (widget, TRUE);
    gtk_widget_show_all (widget);
    gtk_widget_show (gtk_widget_get_toplevel (widget));
    gtk_menu_popup (priv->menu, NULL, NULL, gtk_status_icon_position_menu, status_icon, 1, gdk_event_get_time (event));

    return FALSE;
}

static void
g_paste_applet_status_icon_show_history (GPasteAppletIcon *self)
{
    GPasteAppletStatusIconPrivate *priv = g_paste_applet_status_icon_get_instance_private (G_PASTE_APPLET_STATUS_ICON (self));;
    g_paste_applet_status_icon_popup (priv->icon, gtk_get_current_event (), priv);
}

static void
g_paste_applet_status_icon_dispose (GObject *object)
{
    GPasteAppletStatusIconPrivate *priv = g_paste_applet_status_icon_get_instance_private ((GPasteAppletStatusIcon *) object);

    if (priv->icon)
    {
        g_signal_handler_disconnect (priv->icon, priv->press_id);
        g_clear_object (&priv->icon);
    }
    g_clear_object (&priv->menu);

    G_OBJECT_CLASS (g_paste_applet_status_icon_parent_class)->dispose (object);
}

static void
g_paste_applet_status_icon_class_init (GPasteAppletStatusIconClass *klass)
{
    G_OBJECT_CLASS (klass)->dispose = g_paste_applet_status_icon_dispose;
    G_PASTE_APPLET_ICON_CLASS (klass)->show_history = g_paste_applet_status_icon_show_history;
}

static void
g_paste_applet_status_icon_init (GPasteAppletStatusIcon *self)
{
    GPasteAppletStatusIconPrivate *priv = g_paste_applet_status_icon_get_instance_private (self);

    priv->icon = gtk_status_icon_new_from_icon_name ("edit-paste");
    gtk_status_icon_set_tooltip_text (priv->icon, "GPaste");
    gtk_status_icon_set_visible (priv->icon, TRUE);

    priv->press_id = g_signal_connect (G_OBJECT (priv->icon),
                                       "button-press-event",
                                       G_CALLBACK (g_paste_applet_status_icon_popup),
                                       priv);
}

/**
 * g_paste_applet_status_icon_new:
 * @client: a #GPasteClient
 * @menu: the menu linked to the status icon
 *
 * Create a new instance of #GPasteAppletStatusIcon
 *
 * Returns: a newly allocated #GPasteAppletStatusIcon
 *          free it with g_object_unref
 */
G_PASTE_VISIBLE GPasteAppletIcon *
g_paste_applet_status_icon_new (GPasteClient *client,
                                GtkMenu      *menu)
{
    g_return_val_if_fail (G_PASTE_IS_CLIENT (client), NULL);
    g_return_val_if_fail (GTK_IS_MENU (menu), NULL);

    GPasteAppletIcon *self = g_paste_applet_icon_new (G_PASTE_TYPE_APPLET_STATUS_ICON, client);
    GPasteAppletStatusIconPrivate *priv = g_paste_applet_status_icon_get_instance_private ((GPasteAppletStatusIcon *) self);

    priv->menu = g_object_ref (menu);

    return self;
}
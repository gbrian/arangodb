/*jslint indent: 2, nomen: true, maxlen: 100, vars: true, white: true, plusplus: true */
/*global Backbone, templateEngine, $, window*/
(function () {
    "use strict";
    window.NavigationView = Backbone.View.extend({
        el: '#navigationBar',

        events: {
            "change #arangoCollectionSelect": "navigateBySelect",
            "click .tab": "navigateByTab"
        },

        initialize: function () {
            this.dbSelectionView = new window.DBSelectionView({
                collection: window.arangoDatabase,
                current: window.currentDB
            });
        },

        handleSelectDatabase: function() {
            this.dbSelectionView.render($("#dbSelect"));
        },


        template: templateEngine.createTemplate("navigationView.ejs"),

        render: function () {
            $(this.el).html(this.template.render({
                isSystem: window.currentDB.get("isSystem")
            }));
            this.dbSelectionView.render($("#dbSelect"));
            return this;
        },

        navigateBySelect: function () {
            var navigateTo = $("#arangoCollectionSelect").find("option:selected").val();
            window.App.navigate(navigateTo, {trigger: true});
        },

        navigateByTab: function (e) {
            var tab = e.target || e.srcElement;
            var navigateTo = tab.id;
            if (navigateTo === "links") {
                $("#link_dropdown").slideToggle(200);
                e.preventDefault();
                return;
            }
            if (navigateTo === "tools") {
                $("#tools_dropdown").slideToggle(200);
                e.preventDefault();
                return;
            }
            if (navigateTo === "dbselection") {
                $("#dbs_dropdown").slideToggle(200);
                e.preventDefault();
                return;
            }
            window.App.navigate(navigateTo, {trigger: true});
            e.preventDefault();
        },
        handleSelectNavigation: function () {
            $("#arangoCollectionSelect").change(function () {
                var navigateTo = $(this).find("option:selected").val();
                window.App.navigate(navigateTo, {trigger: true});
            });
        },


        selectMenuItem: function (menuItem) {
            $('.navlist li').removeClass('active');
            if (menuItem) {
                $('.' + menuItem).addClass('active');
            }
        }

    });
}());

using System.Web;
using System.Web.Optimization;

namespace UpdateManager.Web
{
    public class BundleConfig
    {
        // For more information on bundling, visit http://go.microsoft.com/fwlink/?LinkId=301862
        public static void RegisterBundles(BundleCollection bundles)
        {
            bundles.Add(new ScriptBundle("~/bundles/jquery").Include(
                        "~/Scripts/jquery-{version}.js",
                        "~/Scripts/jquery-ui-{version}.js",
                        "~/Scripts/jquery-css-transform.js",
                        "~/Scripts/jquery.easing.1.3.js",
                        "~/Scripts/jquery.quicksand.js",
                        "~/Scripts/custom.quicksand.js"));

            bundles.Add(new ScriptBundle("~/bundles/jqueryval").Include(
                        "~/Scripts/jquery.validate*"));

            // Use the development version of Modernizr to develop with and learn from. Then, when you're
            // ready for production, use the build tool at http://modernizr.com to pick only the tests you need.
            bundles.Add(new ScriptBundle("~/bundles/modernizr").Include(
                        "~/Scripts/modernizr-*"));

            bundles.Add(new ScriptBundle("~/bundles/quicksand").Include(
               "~/Scripts/jquery.quicksand.js",
               "~/Scripts/jquery-css-transform.js",
               "~/Scripts/jquery.easing.1.3.js",               
               "~/Scripts/custom.quicksand.js"));

            bundles.Add(new ScriptBundle("~/bundles/custom-behavior").Include(   
                        //"~/Plugins/eComboBox/jquery.eComboBox.min.js",
                        "~/Plugins/eComboBox/jquery.eComboBox.js",
                        "~/Scripts/custom.js"));

            bundles.Add(new ScriptBundle("~/bundles/bootstrap").Include(
                    "~/Plugins/bootstrap-select/bootstrap-select.min.js",
                    "~/Scripts/bootstrap.js",
                    "~/Scripts/respond.js"));

            bundles.Add(new ScriptBundle("~/bundles/dropzone").Include(
                "~/Plugins/dropzone/dropzone.min.js",
                "~/Scripts/dropzone-updater.js"));           

            bundles.Add(new StyleBundle("~/Content/dropzone").Include(
                "~/Plugins/dropzone/css/basic.css",
                "~/Plugins/dropzone/css/dropzone.css"));            


            bundles.Add(new StyleBundle("~/Content/css").Include(
                      "~/Plugins/bootstrap-select/bootstrap-select.min.css",
                      "~/Content/bootstrap.css",                      
                      "~/Content/themes/base/jquery-ui.css", // TODO: Create separate bundle for jQuery-UI
                      "~/Content/site.css"));
        }
    }
}

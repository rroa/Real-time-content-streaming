using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(UpdateManager.Web.Startup))]
namespace UpdateManager.Web
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}

namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedRolesSupport : DbMigration
    {
        public override void Up()
        {
            AddColumn("UpdateManager.PackageFiles", "Roles", c => c.String(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("UpdateManager.PackageFiles", "Roles");
        }
    }
}

namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedUpdatePackagePriority : DbMigration
    {
        public override void Up()
        {
            AddColumn("UpdateManager.UpdatePackages", "Priority", c => c.Int(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("UpdateManager.UpdatePackages", "Priority");
        }
    }
}
